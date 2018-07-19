//
// Created by zhuanxu on 2018/6/9.
//

#include <x86intrin.h>
#include <cstdio>
#include <iostream>
#include <random>
#include <memory>
#include <chrono>
#include <cstring>

using namespace std;

//__attribute(align(16))
#define _MM_ALIGN16 __attribute__((aligned(16)))
#define _MM_ALIGN32 __attribute__((aligned(32)))


inline double getSum256(__m256 *ymm) {
    _MM_ALIGN32 float tmp[8];
    _mm256_store_ps(tmp, *ymm);
    double result = 0;
    for (int i = 0; i < 8; i++) {
        result += tmp[i];
    }
    return result;
}

inline double getSum128(__m128 *xmm) {
    _MM_ALIGN16 float tmp[4];
    _mm_store_ps(tmp, *xmm);
    double result = 0;
    for (int i = 0; i < 4; i++) {
        result += tmp[i];
    }
    return result;
}

double cosine_similarity_native(const float *a, const float *b, size_t n) {
    // 计算cosin相似度
    double result = 0;
    double suma = 0;
    double sumb = 0;
    for (int i = 0; i < n; ++i) {
        result += a[i] * b[i];
        suma += a[i] * a[i];
        sumb += b[i] * b[i];
    }
    return result / sqrt(suma) / sqrt(sumb);
}

double sigmoid_normal(const float *a, const float *b, size_t n) {
    // 计算cosin相似度
    double result = 0;
    for (int i = 0; i < n; ++i) {
        result += a[i] * b[i];
    }
    return 1 / (1+exp(-result));
}

double test_move(float *a, float *b, size_t n) {
    // 计算cosin相似度
    for (int i = 0; i < n; ++i) {
        a[i] = b[i];
    }
    return a[0];
}
double test_memcopy(float *a, float *b, size_t n) {
    // 计算cosin相似度
    memcpy(a,b, sizeof(float)*n);
    return a[0];
}

double sigmoid_fast(const float *a, const float *b, size_t n) {
    // 计算cosin相似度
    double result = 0;
    for (int i = 0; i < n; ++i) {
        result += a[i] * b[i];
    }
    return result /  ( 1 + fabs(result));
}

double cosine_similarity_int_native(const int *a, const int *b, size_t n) {
    // 计算cosin相似度
    long result = 0;
    long suma = 0;
    long sumb = 0;
    for (int i = 0; i < n; ++i) {
        result += a[i] * b[i];
        suma += a[i] * a[i];
        sumb += b[i] * b[i];
    }
    return result / sqrt(suma) / sqrt(sumb);
}

double cosine_similarity_sse_native(const float *a, const float *b, size_t n) {
    const size_t chunks = n / 4;
    // 计算cosin相似度
    __m128 dots = _mm_setzero_ps();
    __m128 suma = _mm_setzero_ps();
    __m128 sumb = _mm_setzero_ps();
    for (int i = 0; i < n; i += 4) {
        __m128 A = _mm_loadu_ps(&a[i]);
        __m128 B = _mm_loadu_ps(&b[i]);
        __m128 AB = _mm_mul_ps(A, B);
        __m128 AA = _mm_mul_ps(A, A);
        __m128 BB = _mm_mul_ps(B, B);
        dots = _mm_add_ps(dots, AB);
        suma = _mm_add_ps(suma, AA);
        sumb = _mm_add_ps(sumb, BB);
    }
    double result = getSum128(&dots);
    double sumaa = getSum128(&suma);
    double sumbb = getSum128(&sumb);
//    float tmp[4];
//    _mm_storeu_ps(tmp, dots);
//    double result = tmp[0] + tmp[1] + tmp[2] + tmp[3];
//    float tmp1[4];
//    _mm_storeu_ps(tmp1, suma);
//    double sumaa = tmp1[0] + tmp1[1] + tmp1[2] + tmp1[3];
//    float tmp2[4];
//    _mm_storeu_ps(tmp2, sumb);
//    double sumbb = tmp2[0] + tmp2[1] + tmp2[2] + tmp2[3];

    for (size_t i = chunks * 4; i < n; i++) {
        result += a[i] * b[i];
        sumaa += a[i] * a[i];
        sumbb += b[i] * b[i];
    }
    return result / sqrt(sumaa) / sqrt(sumbb);
}

/**
 * 速度和 sse_native 差不读，但是简洁很多
 * @param a
 * @param b
 * @param n
 * @return
 */
double cosine_similarity_sse_dpps(const float *a, const float *b, size_t n) {
    const size_t chunks = n / 4;
    // 计算cosin相似度
    double result = 0;
    double suma = 0;
    double sumb = 0;

    for (int i = 0; i < n; i += 4) {
        __m128 A = _mm_loadu_ps(&a[i]);
        __m128 B = _mm_loadu_ps(&b[i]);
        __m128 AB = _mm_mul_ps(A, B);
        __m128 AA = _mm_mul_ps(A, A);
        __m128 BB = _mm_mul_ps(B, B);
        result += _mm_cvtss_f32(AB);
        suma += _mm_cvtss_f32(AA);
        sumb += _mm_cvtss_f32(BB);
    }

    for (size_t i = chunks * 4; i < n; i++) {
        result += a[i] * b[i];
        suma += a[i] * a[i];
        sumb += b[i] * b[i];
    }
    return result / sqrt(suma) / sqrt(sumb);
}


double cosine_similarity_avx_native(const float *a, const float *b, size_t n) {
    const size_t chunks = n / 8;
    // 计算cosin相似度
    __m256 dots = _mm256_setzero_ps();
    __m256 suma = _mm256_setzero_ps();
    __m256 sumb = _mm256_setzero_ps();
    for (int i = 0; i < n; i += 8) {
        __m256 A = _mm256_loadu_ps(&a[i]);
        __m256 B = _mm256_loadu_ps(&b[i]);
        __m256 AB = _mm256_mul_ps(A, B);
        __m256 AA = _mm256_mul_ps(A, A);
        __m256 BB = _mm256_mul_ps(B, B);
        dots = _mm256_add_ps(dots, AB);
        suma = _mm256_add_ps(suma, AA);
        sumb = _mm256_add_ps(sumb, BB);
    }
    double result = getSum256(&dots);
    double sumaa = getSum256(&suma);
    double sumbb = getSum256(&sumb);
//    float tmp[8];
//    _mm256_storeu_ps(tmp, dots);
//    double result = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7] ;
//    float tmp1[8];
//    _mm256_storeu_ps(tmp1, suma);
//    double sumaa = tmp1[0] + tmp1[1] + tmp1[2] + tmp1[3] + tmp1[4] + tmp1[5] + tmp1[6] + tmp1[7] ;
//    float tmp2[8];
//    _mm256_storeu_ps(tmp2, sumb);
//    double sumbb = tmp2[0] + tmp2[1] + tmp2[2] + tmp2[3] + tmp2[4] + tmp2[5] + tmp2[6] + tmp2[7] ;

    for (size_t i = chunks * 8; i < n; i++) {
        result += a[i] * b[i];
        sumaa += a[i] * a[i];
        sumbb += b[i] * b[i];
    }
    return result / sqrt(sumaa) / sqrt(sumbb);
}



double cosine_similarity_avx_native_align(const float *a, const float *b, size_t n) {
    const size_t chunks = n / 8;
    // 计算cosin相似度
    __m256 dots = _mm256_setzero_ps();
    __m256 suma = _mm256_setzero_ps();
    __m256 sumb = _mm256_setzero_ps();
    for (int i = 0; i < n; i += 8) {
        __m256 A = _mm256_load_ps(&a[i]);
        __m256 B = _mm256_load_ps(&b[i]);
        __m256 AB = _mm256_mul_ps(A, B);
        __m256 AA = _mm256_mul_ps(A, A);
        __m256 BB = _mm256_mul_ps(B, B);
        dots = _mm256_add_ps(dots, AB);
        suma = _mm256_add_ps(suma, AA);
        sumb = _mm256_add_ps(sumb, BB);
    }
    double result = getSum256(&dots);
    double sumaa = getSum256(&suma);
    double sumbb = getSum256(&sumb);
//    float tmp[8];
//    _mm256_storeu_ps(tmp, dots);
//    double result = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7] ;
//    float tmp1[8];
//    _mm256_storeu_ps(tmp1, suma);
//    double sumaa = tmp1[0] + tmp1[1] + tmp1[2] + tmp1[3] + tmp1[4] + tmp1[5] + tmp1[6] + tmp1[7] ;
//    float tmp2[8];
//    _mm256_storeu_ps(tmp2, sumb);
//    double sumbb = tmp2[0] + tmp2[1] + tmp2[2] + tmp2[3] + tmp2[4] + tmp2[5] + tmp2[6] + tmp2[7] ;

    for (size_t i = chunks * 8; i < n; i++) {
        result += a[i] * b[i];
        sumaa += a[i] * a[i];
        sumbb += b[i] * b[i];
    }
    return result / sqrt(sumaa) / sqrt(sumbb);
}


double cosine_similarity_fma_native(const float *a, const float *b, size_t n) {
    const size_t chunks = n / 8;
    // 计算cosin相似度
    __m256 dots = _mm256_setzero_ps();
    __m256 suma = _mm256_setzero_ps();
    __m256 sumb = _mm256_setzero_ps();
    for (int i = 0; i < n; i += 8) {
        __m256 A = _mm256_loadu_ps(&a[i]);
        __m256 B = _mm256_loadu_ps(&b[i]);

//        __m256 AB = _mm256_mul_ps(A, B);
        dots = _mm256_fmadd_ps(A, B, dots);
        suma = _mm256_fmadd_ps(A, A, suma);
        sumb = _mm256_fmadd_ps(B, B, sumb);
//        __m256 AA = _mm256_mul_ps(A, A);
//        __m256 BB = _mm256_mul_ps(B, B);
//        dots = _mm256_add_ps(dots, AB);
//        suma = _mm256_add_ps(suma, AA);
//        sumb = _mm256_add_ps(sumb, BB);
    }
    double result = getSum256(&dots);
    double sumaa = getSum256(&suma);
    double sumbb = getSum256(&sumb);
//    float tmp[8];
//    _mm256_storeu_ps(tmp, dots);
//    double result = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7] ;
//    float tmp1[8];
//    _mm256_storeu_ps(tmp1, suma);
//    double sumaa = tmp1[0] + tmp1[1] + tmp1[2] + tmp1[3] + tmp1[4] + tmp1[5] + tmp1[6] + tmp1[7] ;
//    float tmp2[8];
//    _mm256_storeu_ps(tmp2, sumb);
//    double sumbb = tmp2[0] + tmp2[1] + tmp2[2] + tmp2[3] + tmp2[4] + tmp2[5] + tmp2[6] + tmp2[7] ;

    for (size_t i = chunks * 8; i < n; i++) {
        result += a[i] * b[i];
        sumaa += a[i] * a[i];
        sumbb += b[i] * b[i];
    }
    return result / sqrt(sumaa) / sqrt(sumbb);
}

double cosine_similarity_fma_unroll1(const float *a, const float *b, size_t n) {
    const size_t chunks = n / (8*2);
    // 计算cosin相似度
    __m256 dots[2] = {_mm256_setzero_ps()};
    __m256 suma[2] = {_mm256_setzero_ps()};
    __m256 sumb[2] = {_mm256_setzero_ps()};
    for (int i = 0; i < n; i += 8*2) {
        __m256 A0 = _mm256_loadu_ps(&a[i]);
        __m256 B0 = _mm256_loadu_ps(&b[i]);
        __m256 A1 = _mm256_loadu_ps(&a[i+8]);
        __m256 B1 = _mm256_loadu_ps(&b[i+8]);

        dots[0] = _mm256_fmadd_ps(A0, B0, dots[0]);
        suma[0] = _mm256_fmadd_ps(A0, A0, suma[0]);
        sumb[0] = _mm256_fmadd_ps(B0, B0, sumb[0]);
        dots[1] = _mm256_fmadd_ps(A1, B1, dots[1]);
        suma[1] = _mm256_fmadd_ps(A1, A1, suma[1]);
        sumb[1] = _mm256_fmadd_ps(B1, B1, sumb[1]);
//        __m256 AA = _mm256_mul_ps(A, A);
//        __m256 BB = _mm256_mul_ps(B, B);
//        dots = _mm256_add_ps(dots, AB);
//        suma = _mm256_add_ps(suma, AA);
//        sumb = _mm256_add_ps(sumb, BB);
    }
    double result = getSum256(&dots[0])+ getSum256(&dots[1]);
    double sumaa = getSum256(&suma[0])+ getSum256(&suma[1]);
    double sumbb = getSum256(&sumb[0])+ getSum256(&sumb[1]);
//    float tmp[8];
//    _mm256_storeu_ps(tmp, dots);
//    double result = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7] ;
//    float tmp1[8];
//    _mm256_storeu_ps(tmp1, suma);
//    double sumaa = tmp1[0] + tmp1[1] + tmp1[2] + tmp1[3] + tmp1[4] + tmp1[5] + tmp1[6] + tmp1[7] ;
//    float tmp2[8];
//    _mm256_storeu_ps(tmp2, sumb);
//    double sumbb = tmp2[0] + tmp2[1] + tmp2[2] + tmp2[3] + tmp2[4] + tmp2[5] + tmp2[6] + tmp2[7] ;

    for (size_t i = chunks * 8*2; i < n; i++) {
        result += a[i] * b[i];
        sumaa += a[i] * a[i];
        sumbb += b[i] * b[i];
    }
    return result / sqrt(sumaa) / sqrt(sumbb);
}


template <int BX>
inline double dotWithAVXUnroll(float const *a, float const *b,const int len){
    int step = len / (8*BX);
    __m256 *one = (__m256*)a;
    __m256 *two = (__m256*)b;

    __m256 dots[BX] = {_mm256_setzero_ps()};
    __m256 suma[BX] = {_mm256_setzero_ps()};
    __m256 sumb[BX] = {_mm256_setzero_ps()};
    for (int i=0;i<step;i++){
        for (int j=0;j<BX;j++){
#pragma unroll
            dots[j] = _mm256_fmadd_ps(one[j+BX*i],two[j+BX*i],dots[j]);
            suma[j] = _mm256_fmadd_ps(one[j+BX*i],one[j+BX*i],suma[j]);
            sumb[j] = _mm256_fmadd_ps(two[j+BX*i],two[j+BX*i],sumb[j]);
        }
    }
    for (int j=1;j<BX;j++){
        dots[0] = _mm256_add_ps(dots[0],dots[j]);
        suma[0] = _mm256_add_ps(suma[0],suma[j]);
        sumb[0] = _mm256_add_ps(sumb[0],sumb[j]);
    }
    double result = getSum256(&(dots[0]));
    double sumaa = getSum256(&suma[0]);
    double sumbb = getSum256(&sumb[0]);

    for (size_t i = (8*BX)*step; i < len; i++) {
        result += a[i] * b[i];
        sumaa += a[i] * a[i];
        sumbb += b[i] * b[i];
    }
    return result / sqrt(sumaa) / sqrt(sumbb);
}

double cosine_similarity_fma_unroll(const float *a, const float *b, size_t n){
    return dotWithAVXUnroll<1>(a,b,n);
}

using Clock = std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> dis(1.0, 2.0);
static std::uniform_int_distribution<int> dis_int(-1000, 1000);

class TestCase {
    const size_t SIZE = 5000 * 16;
    float *input1;
    float *input2;
public:
    TestCase() {
        input1 = new float[SIZE];
        input2 = new float[SIZE];
        for (size_t i = 0; i < SIZE; i++) {
            input1[i] = get_random();
            input2[i] = get_random();
        }
    }

    ~TestCase() {
        delete[] input1;
        delete[] input2;
    }

    template<typename FUNCTION>
    void run(const char *name, FUNCTION fun) {
        const size_t ITERATIONS = 15;

        volatile float result = 0.0;
        Clock::rep best_time = std::numeric_limits<Clock::rep>::max();
        for (size_t i = 0; i < ITERATIONS; i++) {
            const auto t1 = Clock::now();
            const float r = fun(input1, input2, SIZE);
            const auto t2 = Clock::now();
            result = r;

            const Clock::rep dt = duration_cast<microseconds>(t2 - t1).count();
            if (dt < best_time) {
                best_time = dt;
            }
//            printf("%-20s: %lu us (result = %0.5f)\n", name, dt, result);
        }

        printf("%-20s: %lu us (result = %0.5f)\n", name, best_time, result);
    }

private:

    float get_random() {
//        std::random_device rd;  //Will be used to obtain a seed for the random number engine
//        std::mt19937 gen(); //Standard mersenne_twister_engine seeded with rd()
        return dis(gen);
    }
};


int main() {
//    printf("%f\n", compute_pi_native(1000));
    TestCase testCase;
    testCase.run("native", cosine_similarity_native);
    testCase.run("SSE (naive)", cosine_similarity_sse_native);
    testCase.run("SSE (DPPS)", cosine_similarity_sse_dpps); // 这种方法不怎么快。
    testCase.run("AVX (naive)", cosine_similarity_avx_native);
    testCase.run("AVX (naive align)", cosine_similarity_avx_native_align);
    testCase.run("FMA (naive)", cosine_similarity_fma_native);
    testCase.run("FMA (unroll1)", cosine_similarity_fma_unroll1);
    testCase.run("FMA (unroll)", cosine_similarity_fma_unroll);

//    {
//        cout << "测试int........" << endl;
//        TestCaseInt testCase;
//        testCase.run("native", cosine_similarity_int_native);
//    }

    {
        cout << "测试sigmoid........" << endl;
        TestCase testCase;
        testCase.run("sigmoid normal", sigmoid_normal);
        testCase.run("sigmoid fast", sigmoid_fast);
    }

    {
        cout << "测试内存拷贝........" << endl;
        TestCase testCase;
        testCase.run("test_move", test_move);
        testCase.run("test_memcopy", test_memcopy);
    }

}