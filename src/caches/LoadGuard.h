#ifndef LOAD_GUARD_H
#define LOAD_GUARD_H


template <typename T>
class LoadGuard {
	
	T* queue;
    size_t capacity;
	size_t tail, size;
    
	void destroy(){
		if(queue != nullptr){
			delete [] queue;
			queue = nullptr;
		}
		capacity = 0;
		tail = 0;
		size = 0;
	}
    
    inline size_t tail_move()
    {
       size_t old_value = tail;
       tail++;
       if(tail > capacity - 1) {
           tail = 0;
       }
       return old_value;
    }
	
public:
		
	LoadGuard(size_t capacity) : queue(new T[capacity]), capacity(capacity), tail(0), size(0) {
	}
	
	virtual ~LoadGuard() {
		destroy();
	}
	
	LoadGuard(const LoadGuard&) = delete;
	LoadGuard& operator=(const LoadGuard&) = delete;
    
	LoadGuard(LoadGuard&& rvalue) = delete;
	LoadGuard& operator=(LoadGuard&& rvalue) = delete;
	
	bool push(const T& value, T* oldest) noexcept {
		if(size < capacity){
            queue[tail_move()] = value;
			size++;
			return false;
		} else {
            *oldest = queue[tail];
            queue[tail_move()] = value;
            return true;
        }
	}
	
	void reset() noexcept {
		size = 0;
	}
	
};

#endif /* LOAD_GUARD_H */

