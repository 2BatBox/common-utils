#include "LruCacheTest.h"
#include "TimerQueueTest.h"

#include <iostream>

typedef unsigned Key_t;
typedef unsigned Value_t;

int main(int, char**)
{
    unsigned storage_size = 1024 * 1024;
    float load_factor = 0.7f;
    
    cache::LruCacheTest cache_test(storage_size, load_factor);
    cache_test.test();
    std::cout << "\n";

    cache::TimerQueueTest time_cache_test(storage_size, load_factor);
    time_cache_test.test();
    std::cout << "\n";
    
    std::cout << "<---- the end of main() ---->\n";
    return 0;
}

