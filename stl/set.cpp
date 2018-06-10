//
// Created by zhuanxu on 2018/6/8.
//

#include <unordered_set>
#include <iostream>
using namespace std;

int main() {
    unordered_set<int> a;
    a.insert(1);
    a.insert(2);
    a.insert(3);

    cout << (*a.begin()) << endl;
}