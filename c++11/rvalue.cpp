//
// Created by zhuanxu on 2018/5/21.
//


#include <iostream>

class A {
public:
    int *pointer;

    A() : pointer(new int(1)) { std::cout << "构造" << pointer << std::endl; }

    A(A &a) : pointer(new int(*a.pointer)) { std::cout << "拷贝" << pointer << std::endl; } // 无意义的对象拷贝
    A(A &&a) : pointer(a.pointer) {
        a.pointer = nullptr;
        std::cout << "移动" << pointer << std::endl;
    }

    ~A() {
        std::cout << "析构" << pointer << std::endl;
        delete pointer;
    }
};

// 防止编译器优化
A return_rvalue(bool test) {
    A a, b;
    if (test) return a;
    else return b;
}

int main() {
    /**
    构造0x7fd4f4400340
    构造0x7fd4f4500000
    移动0x7fd4f4500000
    析构0
    析构0x7fd4f4400340
    obj:
    0x7fd4f4500000
    1
    析构0x7fd4f4500000
     */
    A obj = return_rvalue(false);
    std::cout << "obj:" << std::endl;
    std::cout << obj.pointer << std::endl;
    std::cout << *obj.pointer << std::endl;
    return 0;
}
