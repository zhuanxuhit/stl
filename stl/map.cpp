//
// Created by zhuanxu on 2018/6/27.
//


#include <unordered_map>
#include <iostream>

using namespace std;

void test_map_key_ptr() {
    int a = 1;
    int b = 2;
    unordered_map<int *, int> maps;
    maps[&a] = a;
    maps[&b] = b;
    cout << maps[&a] << " " << maps[&b] << endl;
}

int main() {
    test_map_key_ptr();
}