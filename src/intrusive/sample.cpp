#include <iostream>

#include "intrusive/ListTest.h"
#include "intrusive/MapTest.h"

typedef unsigned Key_t;
typedef unsigned Value_t;

int main(int, char**)
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