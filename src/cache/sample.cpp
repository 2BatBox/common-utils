#include "LruCacheTest.h"
#include "TimeCacheTest.h"

#include <iostream>

typedef unsigned Key_t;
typedef unsigned Value_t;

int sample_cache(int, char**)
{
    unsigned storage_size = 1024 * 1024;
    float load_factor = 0.7f;
    
    cache::LruCacheTest cache_test(storage_size, load_factor);
    cache_test.test();
    std::cout << "\n";

    cache::TimeCacheTest time_cache_test(storage_size, load_factor);
    time_cache_test.test();
    std::cout << "\n";
    
    std::cout << "<---- the end of main() ---->\n";
    return 0;
}

