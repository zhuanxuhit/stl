//
// Created by Wang,Chao(EBDPRD02) on 2018/5/14.
//
#pragma once

#include <iterator>

template<typename T, typename Outer>
class combination_iterator :
        public std::iterator<std::input_iterator_tag, combination_iterator<T, Outer>> {
private:
    T value;
    T begin;
    T end;
    Outer outer;
public:
    typedef std::iterator<std::input_iterator_tag, combination_iterator> base;

    combination_iterator(T const &begin, T const &end, Outer const &outer) : value(begin), begin(begin), end(end),
                                                                             outer(outer) {}

    combination_iterator &
    operator++() {
        ++value;
        if (value == end) {
            ++outer;
            if (!outer.is_end()) value = begin;
        }
        return *this;
    }

    bool is_end() const { return (value == end) && outer.is_end(); }

    typename base::reference
    operator*() { return *this; }

    bool operator==(const combination_iterator &rhs) const {
        return is_end() && rhs.is_end();
    }

    bool operator!=(const combination_iterator &rhs) const {
        return !(*this == rhs);
    }

    T const &get_value() const {
        return value;
    }

    Outer const &get_outer() const {
        return outer;
    }
};

// 生成组合迭代器的助手函数
template<typename T, typename Outer>
combination_iterator<T, Outer>
combinator(T const &begin, T const &end, Outer const &outer) {
    return combination_iterator<T, Outer>(begin, end, outer);
};

// 生成一个处于"结束"状态的组合迭代器
template<typename T, typename Outer>
combination_iterator<T, Outer>
combinator(Outer const &outer) {
    return combination_iterator<T, Outer>(T(), T(), outer);
};


// 对于 Outer为 void 情况下的特化
template<typename T>
class combination_iterator<T, void> :
        public std::iterator<std::input_iterator_tag, combination_iterator<T, void>> {
private:
    T value;
    T begin;
    T end;
public:
    typedef std::iterator<std::input_iterator_tag, combination_iterator> base;

    combination_iterator(T const &begin, T const &end) : value(begin), begin(begin), end(end) {}

    combination_iterator &
    operator++() {
        ++value;
        return *this;
    }

    bool is_end() const { return (value == end); }

    typename base::reference
    operator*() { return *this; }

    bool operator==(const combination_iterator &rhs) const {
        return is_end() && rhs.is_end();
    }

    bool operator!=(const combination_iterator &rhs) const {
        return !(*this != rhs);
    }

    T const &get_value() const {
        return value;
    }
};

// 生成组合迭代器的助手函数
template<typename T>
combination_iterator<T, void>
combinator(T const &begin, T const &end) {
    // 特化
    return combination_iterator<T, void>(begin, end);
};

// 生成一个处于"结束"状态的组合迭代器
template<typename T>
combination_iterator<T, void>
combinator() {
    return combination_iterator<T, void>(T(), T());
};

