#ifndef STORAGE_H
#define STORAGE_H

template <typename K, typename V>
class Storage {
    std::unordered_map<K, V> table;
    LoadGuard<K> load_guard;
    std::mutex lock;

public:

    Storage(size_t capacity, float load_factor) : table(), load_guard(capacity), lock()
    {
        table.max_load_factor(load_factor);
        table.reserve(capacity);
    }

    void put(const K& key, const V& value)
    {
        std::lock_guard<std::mutex> lk(lock);
        table.insert(std::pair<K, V>(key, value));

        K old_key;
        if (load_guard.push(key, &old_key)) {
            table.erase(old_key);
        }
    }

    bool get(const K& key, V& value)
    {
        std::lock_guard<std::mutex> lk(lock);
        auto it = table.find(key);
        if (it != table.end()) {
            value = (*it).second;
            return true;
        }
        return false;
    }

    void dump()
    {
        std::cout << "size:            " << table.size() << "\n";
        std::cout << "buckets:         " << table.bucket_count() << "\n";
        std::cout << "load factor:     " << table.load_factor() << "\n";
        std::cout << "max load factor: " << table.max_load_factor() << "\n";
        std::cout << "data: \n";

        for (decltype(table.bucket_count()) bucket = 0; bucket < table.bucket_count(); ++bucket) {
            std::cout << "b[" << std::setw(2) << bucket << "]: (" << table.bucket_size(bucket) << ") ";
            for (auto it = table.cbegin(bucket); it != table.cend(bucket); ++it) {
                std::cout << (*it).first << ":" << (*it).second << "  ";
            }
            std::cout << "\n";
        }
    }

};

#endif /* STORAGE_H */

