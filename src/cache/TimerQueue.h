#ifndef CACHE_TIMER_QUEUE_H
#define CACHE_TIMER_QUEUE_H

#include <ctime>
#include <bits/allocator.h>

#include "../intrusive/List.h"
#include "../intrusive/Map.h"

// gcc 4.8.2's -Wnon-virtual-dtor is broken and turned on by -Weffc++
#if __GNUC__ < 3 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"
#define GCC_DIAG_POP_NEEDED
#endif

namespace cache {

template <typename K, typename V>
struct TimerQueueData : public intrusive::ListHook<TimerQueueData<K, V> >, intrusive::MapHook<K, TimerQueueData<K, V> > {
    typedef K Key_t;
    typedef V Value_t;
    V value;
    std::time_t time;

    TimerQueueData() : value(), time() { }

    TimerQueueData(const V& v) : value(v), time() { }

    bool operator==(const TimerQueueData& data) const
    {
        return value == data.value;
    }
};

template <
typename TimerQueueData_t,
typename H = std::hash<typename TimerQueueData_t::Key_t>,
typename SA = std::allocator<TimerQueueData_t>,
typename BA = std::allocator<intrusive::MapBucket<TimerQueueData_t> >
>
class TimerQueue {
    friend class TimerQueueTest;

    typedef typename TimerQueueData_t::Key_t Key_t;
    typedef typename TimerQueueData_t::Value_t Value_t;
    typedef intrusive::List<TimerQueueData_t > List_t;
    typedef intrusive::Map<Key_t, TimerQueueData_t, H, BA> Map_t;
    typedef typename Map_t::Bucket_t Bucket_t;

    template<typename V>
    struct Iterator {

        Iterator() noexcept : value(nullptr) { }

        Iterator(V* value) noexcept : value(value) { }

        bool operator==(const Iterator& it) const noexcept
        {
            return value == it.value;
        }

        bool operator!=(const Iterator& it) const noexcept
        {
            return value != it.value;
        }

        V& operator*() noexcept
        {
            return *value;
        }

        V* operator->() noexcept
        {
            return value;
        }

        const V& operator*() const noexcept
        {
            return *value;
        }

        const V* operator->() const noexcept
        {
            return value;
        }

    private:
        V* value;
    };

    typedef Iterator<Value_t> Iterator_t;
    typedef Iterator<const Value_t> ConstIterator_t;

    const size_t cache_capacity;
    TimerQueueData_t* storage;
    Map_t map;
    List_t list_cached;
    List_t list_freed;
    SA allocator;

public:

    TimerQueue(unsigned capacity, float load_factor) noexcept
    : cache_capacity(capacity),
    storage(nullptr),
    map((capacity / load_factor) + 1),
    list_cached(),
    list_freed(),
    allocator() { }

public:

    TimerQueue(const TimerQueue&) = delete;
    TimerQueue& operator=(const TimerQueue&) = delete;

    TimerQueue(TimerQueue&& rv) = delete;
    TimerQueue& operator=(TimerQueue&&) = delete;

    virtual ~TimerQueue() noexcept
    {
        destroy();
    }

    bool allocate()
    {
        if (storage)
            return false;

        storage = allocator.allocate(cache_capacity);
        if (storage == nullptr)
            return false;

        TimerQueueData_t empty;
        for (unsigned i = 0; i < cache_capacity; i++) {
            allocator.construct(storage + i, empty);
            list_freed.push_back(storage[i]);
        }

        if (not map.allocate()) {
            destroy();
            return false;
        }

        return true;
    }

    /**
     * @param key
     * @param value
     * @return Returns true if a new record has been added or updated,
     * false - the queue has no space to append a new record.
     */
    bool push_back(Key_t key, const Value_t& value) noexcept
    {
        std::time_t time = std::time(nullptr);
        auto cell = map.find(key);
        if (cell == map.end()) {
            TimerQueueData_t* freed;
            if (list_freed.size()) {
                freed = list_freed.pop_front();
                freed->value = value;
                freed->time = time;
                list_cached.push_back(*freed);
                map.put(key, *freed);
                return true;
            }
        } else {
            cell->value = value;
            cell->time = time;
            list_cached.remove(*cell);
            list_cached.push_back(*cell);
            return true;
        }
        return false;
    }

    /**
     * head                         tail 
     * |--cells to pop-|             |
     * |C|C|C|C|C|C|C|C|C|C|C|C|C|C|C|
     *                 ^---timeout---^
     *                               |
     *                              now
     * @param key_row
     * @param value_row
     * @param row_size
     * @param time 
     * @return 
     */
    bool pop_front(Key_t& key, Value_t& value, int timeout) noexcept
    {
        std::time_t time = std::time(nullptr) - timeout;
        auto it = list_cached.cbegin();
        if (it != list_cached.cend() && (*it).time < time) {
            TimerQueueData_t* cell = list_cached.pop_front();
            key = cell->im_key;
            value = cell->value;
            map.remove(cell->im_key);
            list_freed.push_back(*cell);
            return true;
        }
        return false;
    }

    Iterator_t find(const Key_t& key) noexcept
    {
        auto cell = map.find(key);
        if (cell == map.end())
            return Iterator_t();
        else
            return Iterator_t(&(cell->value));
    }

    ConstIterator_t find(const Key_t& key) const noexcept
    {
        auto cell = map.find(key);
        if (cell == map.end())
            return ConstIterator_t();
        else
            return ConstIterator_t(&(cell->value));
    }

    Iterator_t remove(const Key_t& key) noexcept
    {
        auto cell = map.find(key);
        if (cell != map.end()) {
            map.remove(key);
            list_cached.remove(*cell);
            list_freed.push_back(*cell);
            return Iterator_t(&(cell->value));
        }
        return Iterator_t();
    }

    void reset() noexcept
    {
        map.clear();
        list_cached.clear();
        for (unsigned i = 0; i < cache_capacity; i++) {
            list_freed.push_back(storage[i]);
        }
    }

    size_t capacity() const noexcept
    {
        return cache_capacity;
    }

    size_t size() const noexcept
    {
        return map.size();
    }

    Iterator_t end() noexcept
    {
        return Iterator_t();
    }

    ConstIterator_t cend() const noexcept
    {
        return ConstIterator_t();
    }

    size_t storage_bytes() const noexcept
    {
        return cache_capacity * sizeof(TimerQueueData_t) + map.buckets() * sizeof(Bucket_t);
    }

private:

    void destroy() noexcept
    {
        if (storage) {

            list_freed.clear();
            list_cached.clear();
            map.clear();

            for (size_t i = 0; i < cache_capacity; i++) {
                allocator.destroy(storage + i);
            }
            allocator.deallocate(storage, cache_capacity);
            storage = nullptr;
        }
    }

};

}; // namespace cache

#if defined(GCC_DIAG_POP_NEEDED)
#pragma GCC diagnostic pop
#undef GCC_DIAG_POP_NEEDED
#endif

#endif /* CACHE_TIMER_QUEUE_H */

