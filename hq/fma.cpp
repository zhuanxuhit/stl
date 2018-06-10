//
// Created by zhuanxu on 2018/6/10.
//
#include <x86intrin.h>
#include <cstdio>
#include <iostream>

using namespace std;

//__attribute(align(16))
#define _MM_ALIGN16 __attribute__((aligned(16)))
#define _MM_ALIGN32 __attribute__((aligned(32)))

void fma() {
    cout << "test vector_add ....." << endl;
    _MM_ALIGN32 float op1[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    _MM_ALIGN32 float op2[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    _MM_ALIGN32 float result[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    __m256 a;
    __m256 b;
    __m256 c;
    // Load
    a = _mm256_loadu_ps(op1);
    b = _mm256_loadu_ps(op2);
    c = _mm256_loadu_ps(result);

    // Calculate
    c = _mm256_fmadd_ps(a, b, c);   // c = a + b

    // Store
    _mm256_storeu_ps(result, c);

//    printf("0: %lf\n", c.m128_f32[0]);
//    printf("1: %lf\n", c.m128_f32[1]);
//    printf("2: %lf\n", c.m128_f32[2]);
//    printf("3: %lf\n", c.m128_f32[3]);

    for (int i = 0; i < 8; i++) {
        printf("%d: %f\n", i, result[i]);
    }
}


int main() {
    fma();
}