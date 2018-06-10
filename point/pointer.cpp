//
// Created by zhuanxu on 2018/6/5.
//

#include <iostream>
using std::cout;
using std::endl;

namespace test_01 {
    void test_array(){
        int arr[10] = {1,2,3};
        // 此处 parr 只保留了 arr 是一个指针的信息
        // arr 其实是一个 指向有10个int的数组的指针
        int *parr = arr;
        cout << sizeof(arr) << " " << sizeof(parr) << endl;
        // output: 40 8
        // 正确的指针应该是
        int (*p)[10] = &arr;
        cout << sizeof(p) << endl;
        cout << p << endl; // 此处打印出来是数组arr的首地址
        cout << *p << endl; // 此处打印出来是数组arr的首地址
    }

    void test_double_array(){
        int dou_arr[5][3];
        // 可以看到
    }
}

int main(int argc, char* argv[]){
    test_01::test_array();
    float  a = 1;
    cout << sizeof(a) << endl;
    return 0;
}
