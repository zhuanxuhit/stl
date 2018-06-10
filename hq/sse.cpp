//
// Created by zhuanxu on 2018/6/8.
//

#include <x86intrin.h>
#include <cstdio>
#include <iostream>

using namespace std;

//__attribute(align(16))
#define _MM_ALIGN16 __attribute__((aligned(16)))
#define _MM_ALIGN32 __attribute__((aligned(32)))

void vector_add() {
    cout << "test vector_add ....." << endl;
    _MM_ALIGN16 float op1[4] = {1.0, 2.0, 3.0, 4.0};
    _MM_ALIGN16 float op2[4] = {1.0, 2.0, 3.0, 4.0};
    _MM_ALIGN16 float result[4];
    __m128 a;
    __m128 b;
    __m128 c;
    // Load
    a = _mm_loadu_ps(op1);
    b = _mm_loadu_ps(op2);

    // Calculate
    c = _mm_add_ps(a, b);   // c = a + b

    // Store
    _mm_storeu_ps(result, c);

//    printf("0: %lf\n", c.m128_f32[0]);
//    printf("1: %lf\n", c.m128_f32[1]);
//    printf("2: %lf\n", c.m128_f32[2]);
//    printf("3: %lf\n", c.m128_f32[3]);


    printf("0: %f\n", result[0]);
    printf("1: %f\n", result[1]);
    printf("2: %f\n", result[2]);
    printf("3: %f\n", result[3]);
}

void vector_mul() {
    cout << "test vector_mul ....." << endl;
    _MM_ALIGN16 float op1[4] = {1.0, 2.0, 3.0, 4.0};
    _MM_ALIGN16 float op2[4] = {1.0, 2.0, 3.0, 4.0};
    _MM_ALIGN16 float result[4];
    __m128 a;
    __m128 b;
    __m128 c;
    // Load
    a = _mm_loadu_ps(op1);
    b = _mm_loadu_ps(op2);

    // Calculate
    c = _mm_mul_ps(a, b);   // c = a + b

    // Store
    _mm_storeu_ps(result, c);

//    printf("0: %lf\n", c.m128_f32[0]);
//    printf("1: %lf\n", c.m128_f32[1]);
//    printf("2: %lf\n", c.m128_f32[2]);
//    printf("3: %lf\n", c.m128_f32[3]);


    printf("0: %f\n", result[0]);
    printf("1: %f\n", result[1]);
    printf("2: %f\n", result[2]);
    printf("3: %f\n", result[3]);
}

void vector_dot() {
    cout << "test vector_dot ....." << endl;
    _MM_ALIGN16 float op1[4] = {1.0, 2.0, 3.0, 4.0};
    _MM_ALIGN16 float op2[4] = {1.0, 2.0, 3.0, 4.0};
    _MM_ALIGN16 float result[4] = {0.0, 0.0, 0.0, 0.0};

    __m128 a;
    __m128 b;
    a = _mm_load_ps(op1);
    b = _mm_load_ps(op2);

    __m128 c = _mm_mul_ps(a, b);
    __m128 zero = _mm_setzero_ps();
    c = _mm_hadd_ps(c, zero);
    c = _mm_hadd_ps(c, zero);

    _mm_store_ss(result, c);
    printf("0: %f\n", result[0]);
    printf("1: %f\n", result[1]);
    printf("2: %f\n", result[2]);
    printf("3: %f\n", result[3]);

}

void vector_dot2() {
    cout << "test vector_dot2 ....." << endl;
    _MM_ALIGN16 float op1[4] = {1.0, 2.0, 3.0, 4.0};
    _MM_ALIGN16 float op2[4] = {1.0, 2.0, 3.0, 4.0};
    _MM_ALIGN16 float result[4] = {0.0, 0.0, 0.0, 0.0};

    __m128 a;
    __m128 b;
    a = _mm_load_ps(op1);
    b = _mm_load_ps(op2);

    __m128  c;
    const int mask = 1 + (1<<4) + (1<<5) + (1<<6) + (1<<7);
    // 需要优化选项 -m32 -msse4.1
    c = _mm_dp_ps(a,b,mask);
    _mm_store_ps(result, c);
    printf("0: %f\n", result[0]);
    printf("1: %f\n", result[1]);
    printf("2: %f\n", result[2]);
    printf("3: %f\n", result[3]);
}

int main(int argc, char *argv[]) {
    vector_add();
    vector_mul();
    vector_dot();
    vector_dot2();
}