#include <map>
#include <string>
#include <iostream>
#include <vector>

#include "intrusive/IntrusiveList.h"

template <typename T>
struct Data : public IntrusiveList<Data<T> >::Hook {
    T value;

    Data(T v) : value(v)
    {
    }
};

template <typename T>
std::ostream& operator <<(std::ostream& os, const Data<T>& data)
{
    os << data.value;
    return os;
}

template <typename T>
void dump(const IntrusiveList<T>& list)
{
    std::cout << "list has " << list.size() << " elements \n";
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
}

template <typename T>
void rdump(const IntrusiveList<T>& list)
{
    std::cout << "list has " << list.size() << " elements \n";
    for (auto it = list.crbegin(); it != list.crend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
}

template <typename T>
void filter(IntrusiveList<T>& list)
{
    for (auto it = list.begin(); it != list.end();) {
//        std::cout << *it << " ";
        if(it->value % 5 == 0){
            auto it_tmp = it;
            it++;
            list.remove(&(*it_tmp));
//            std::cout << "rm:" << *it_tmp << " \n";
        } else {
            ++it;
        }
    }
    std::cout << "\n";
}

int main(int, char**)
{
    unsigned BUF_SIZE = 32;
    typedef Data<unsigned> Data_t;
    std::vector<Data_t> vec;
    vec.reserve(BUF_SIZE);

    IntrusiveList<Data_t> list;

    for (unsigned i = 0; i < BUF_SIZE; i++) {
        Data_t data(i);
        vec.push_back(data);
        if (i % 2 == 0)
            list.push_back(&(vec.at(i)));
        else
            list.push_front(&(vec.at(i)));
    }
    
    dump(list);
    filter(list);
    dump(list);
    rdump(list);

    std::cout << "<---- the end of main() ---->\n";
    return 0;
}

