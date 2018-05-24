//
// Created by zhuanxu on 2018/5/24.
//


#include <memory>
#include <iostream>

using std::cout;
using std::endl;


namespace smart_ptr1 {
    class Foo {
    public:
        Foo() {
            cout << "Foo constructor" << endl;
        }
        ~Foo(){
            cout << "Foo destructor" << endl;
        }
    };
    void test_shared_ptr() {
        // 正常构造函数
        std::shared_ptr<Foo> foo(new Foo);
        cout << foo.use_count() << endl;
        // 拷贝构造
        {
            std::shared_ptr<Foo> foo1(foo);
            cout << foo.use_count() << endl;
        }
        cout << foo.use_count() << endl;
        // 赋值构造
        {
            std::shared_ptr<Foo> foo2 = foo;
            cout << foo.use_count() << endl;
        }
        // 移动赋值
        {
            std::shared_ptr<Foo> foo3=std::move(foo);
            cout << foo.use_count() << endl;
        }
        // 移动构造
        {
            std::shared_ptr<Foo> foo3(std::move(foo));
            cout << foo.use_count() << endl;
        }
        cout << foo.use_count() << endl;
    }
}

int main(){
    smart_ptr1::test_shared_ptr();
}