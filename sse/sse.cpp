//
// Created by zhuanxu on 2018/6/6.
//


#include <x86intrin.h>
#include <functional>
#include "Timing.h"
#define _MM_ALIGN16           __attribute__((aligned(16)))

namespace sse_test01 {
    void test_normal() {
        float input1[4] = { 1.2f, 3.5f, 1.7f, 2.8f };
        float input2[4] = { -0.7f, 2.6f, 3.3f, -0.8f };
        float output[4];
        for(int i = 0; i < 4; i++) {
            output[i] = input1[i] + input2[i];
        }
    };
    void test_sse() {
        float input1[4] = { 1.2f, 3.5f, 1.7f, 2.8f };
        float input2[4] = { -0.7f, 2.6f, 3.3f, -0.8f };
        float output[4];
        __m128 a = _mm_loadu_ps(input1);
        __m128 b = _mm_loadu_ps(input2);
        __m128 t = _mm_add_ps(a, b);
        _mm_store_ps(output, t);
    }
    void test_aliged() {
//        _MM_ALIGN16 float intput1[4];
        _MM_ALIGN16 float input1[4] = { 1.2f, 3.5f, 1.7f, 2.8f };
        _MM_ALIGN16 float input2[4] = { -0.7f, 2.6f, 3.3f, -0.8f };
        _MM_ALIGN16 float output[4];
        __m128 a = _mm_load_ps(input1);
        __m128 b = _mm_load_ps(input2);
        __m128 t = _mm_add_ps(a, b);
        _mm_store_ps(output, t);
    }
    void cal_num(int num, std::function<void()> cal) {
        printf("test start:\n");
        startTiming();
        for (int i=0;i<num;i++){
            cal();
        }
        stopWithPrintTiming();
    }
}

int main() {
    const int num = 100000;
    sse_test01::cal_num(num,sse_test01::test_normal);
    sse_test01::cal_num(num,sse_test01::test_sse);
    sse_test01::cal_num(num,sse_test01::test_aliged);
    printf("%f\n",2.0f);
}
