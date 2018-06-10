//
// Created by zhuanxu on 2018/5/27.
//


#include <cstdint>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <iomanip>

using std::cout;
using std::endl;

typedef unsigned char byte;

int h(int32_t n){
    return (n << 1) ^ (n >> 31);
}

int encodeInt(int32_t n, std::vector<byte> &bytes) {
    // 左移1位（n << 1），无论正数还是负数，相当于乘以2；
    // 对于正数，若大于 INT32_MAX / 2 = 1073741823
    // 则会发生溢出，导致左移1位后为负数
    // 右移31位（n >> 31），对于正数，则返回0x00000000；对于负数，则返回0xffffffff
    n = h(n);
    // n = -1, 0x ff ff ff ff
    // n << 1,  0x ff ff ff fe = -2
    // n >> 31, 0x ff ff ff ff
    // (n << 1) ^ (n >> 31) = 1
    while((n & ~0x7f) != 0){ // n > 0x7f
        bytes.push_back((n | 0x80) & 0xff);
//        start++;
        n = n >> 7;
    }
    // 如果 n !=0
    if ((n & 0x7f) != 0){
        bytes.push_back(n);
    }
    return bytes.size();
}

int main() {
    int32_t n = INT32_MAX;
    cout << n << endl;
    cout << n / 2 << endl;
    cout << (n << 1) << endl;

    cout << "zigzag start ....\n";
    std::vector<byte > zigzags;
    n = 64;
    cout << std::hex << n << endl;
    cout << std::hex << h(n) << endl;

    cout << encodeInt(n, zigzags) << endl;
    cout << zigzags.size() << endl;
//    cout << (int32_t) zigzags[0] << endl;
    cout << "0x ";
    for (auto h : zigzags){
        cout << std::hex << std::setw(2) << std::setfill('0') << (int16_t)h << " ";
    }
    cout << endl;
}