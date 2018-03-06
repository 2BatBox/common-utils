#ifndef INTRUSIVE_SAMPLE_H
#define INTRUSIVE_SAMPLE_H

#include <iostream>

#include "ListTest.h"
#include "MapTest.h"

typedef unsigned Key_t;
typedef unsigned Value_t;

int intrusive_sample(int, char**)
{
    unsigned storage_size = 1024 * 1024;
    float load_factor = 0.7f;
    intrusive::ListTest list_test(storage_size);
    list_test.test();
    std::cout << "\n";
    
    intrusive::MapTest map_test(storage_size, load_factor);
    map_test.test();
    std::cout << "\n";
    
    std::cout << "<---- the end of main() ---->\n";
    return 0;
}

#endif /* INTRUSIVE_SAMPLE_H */

