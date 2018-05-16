//
// Created by Wang,Chao(EBDPRD02) on 2018/5/14.
//
#include <iostream>
#include <algorithm>

#include "combination_iterator.h"

struct print_combination {
    template<typename T>
    void operator()(T const &ci) {
        std::cout << ci.get_value() << ","
                  << ci.get_outer().get_value() << ","
                  << *(ci.get_outer().get_outer()).get_value()
                  << std::endl;
    }
};
void print_all_combination() {
    const char *array[] = { // array 是一个数组，每个元素都是一个 const char* 指针
            "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"
    };
    using std::for_each;
    for_each(combinator(0,3,
                        combinator('a','d', combinator(&array[0],&array[7]))),
                combinator<int>(combinator<char>(combinator<const char**>())),
                        print_combination()
            );
}

int main() {
    print_all_combination();
}

