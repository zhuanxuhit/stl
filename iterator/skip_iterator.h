//
// Created by Wang,Chao(EBDPRD02) on 2018/5/11.
//

#pragma once

#include <iterator>

template<typename T>
class skip_iterator : public std::iterator<std::random_access_iterator_tag, T> {
    T *pos;
    size_t step;
public:
    typedef std::iterator<std::random_access_iterator_tag, T> base_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::reference reference;

    // 构造函数
    skip_iterator(T *pos, size_t step) : pos(pos), step(step) {}

    skip_iterator(const skip_iterator &i) : pos(i.pos), step(i.step) {}

    difference_type
    operator-(skip_iterator r) { return (pos - r.pos) / step; }

    skip_iterator
    operator+(typename base_type::difference_type n) {
        return skip_iterator(pos + n * step);
    }

    skip_iterator
    operator++() {
        pos += step;
        return *this;
    }

    bool operator==(const skip_iterator &rhs) const {
        return  pos == rhs.pos;
    }

    bool operator!=(const skip_iterator &rhs) const {
        return !(rhs == *this); // 调用重载的 ==
    }
    // 去引用
    reference operator *() { return  *pos;}
};
