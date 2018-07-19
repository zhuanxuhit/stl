//
// Created by zhuanxu on 2018/7/3.
//
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>

#define THREADS 32
#define START 0
#define END 100000

int main ( ) {
    unsigned long sum = 0, i;

#pragma omp parallel for num_threads(THREADS) reduction(+:sum)
    for (i = START; i <= END; i++) {
        sum += i;
    }

    printf("Final answer = %lu.\n", sum);
    return 0;
}
