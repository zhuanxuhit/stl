//
// Created by Wang,Chao(EBDPRD02) on 2018/5/14.
//

#pragma once

#include <iterator>

template <typename T, typename BinFunc>
class accumulate_iterator:public std::iterator<std::output_iterator_tag, T>{
    T &ref_x;
    BinFunc bin_func;
public:
    accumulate_iterator(T &ref_x, BinFunc bin_func) : ref_x(ref_x), bin_func(bin_func) {}
    // 去引用操作返回自身
    accumulate_iterator operator*() { return *this;}
    // 赋值操作实现累计
    template <typename T0>
    T const &operator=(T0 const &v){
        ref_x=bin_func(ref_x, v);
        return ref_x;
    }
    accumulate_iterator&operator++(){ return  *this;}
};
template <typename T, typename BinFunc>
accumulate_iterator<T,BinFunc>
accumulater(T &ref_x, BinFunc bin_func) {
    return accumulate_iterator<T,BinFunc>(ref_x, bin_func);
};