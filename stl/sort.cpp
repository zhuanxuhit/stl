//
// Created by zhuanxu on 2018/6/6.
//

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

int main() {

    std::vector<int> a = {4, 3, 2, 1};
    std::sort(a.begin(), a.end(), [](int &first, int &second) {
        return first < second;
    });
    for (auto i:a) {
        cout << i << endl;
    }
}

