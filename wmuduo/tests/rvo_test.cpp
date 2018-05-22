//
// Created by Wang,Chao(EBDPRD02) on 2018/5/22.
//


#include <iostream>

using namespace std;

struct Foo
{
    Foo() { cout << "Foo ctor" << endl; }
    Foo(const Foo&) { cout << "Foo copy ctor" << endl; }
    void operator=(const Foo&) { cout << "Foo operator=" << endl; }
    ~Foo() { cout << "Foo dtor" << endl; }
};

Foo make_foo()
{
    /**
    Foo ctor
    Foo dtor
     RVO即Return Value Optimization，
     是一种编译器优化技术，可以把通过函数返回创建的临时对象给”去掉”，
     然后可以达到少调用拷贝构造的操作。
 */
    Foo f;
    return f;
    //return Foo();
}


int main(void)
{
    Foo f = make_foo();
    return 0;
}