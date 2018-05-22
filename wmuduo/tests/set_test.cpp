//
// Created by Wang,Chao(EBDPRD02) on 2018/5/22.
//

#include <set>
#include <iostream>

using namespace std;

int main(void)
{
    int a[] = { 1, 2, 3, 4, 5};
    set<int> s(a, a+5);
    // 找出 >=2的值
    cout<<*s.lower_bound(2)<<endl;
    // 找出 > 2 的值
    cout<<*s.upper_bound(2)<<endl;
    return 0;
}
