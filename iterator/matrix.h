//
// Created by Wang,Chao(EBDPRD02) on 2018/5/14.
//

#pragma once

#include "skip_iterator.h"
#include "accumulate_iterator.h"
#include <algorithm>

template<typename T>
class matrix {
public:
    typedef T value_type;
    typedef T *iterator;
    typedef T *row_iterator;
    typedef skip_iterator<T> col_iterator;
    // 只读迭代器
    typedef const T *const_iterator;
    typedef const T *const_row_iterator;
    typedef skip_iterator<const T> const_col_iterator;

private:
    T *data;
    size_t n_row;
    size_t n_col;
public:
    // 构造
    matrix(size_t n_row, size_t n_col) :
            data(new T[n_col * n_row]), n_row(n_row), n_col(n_col) {}

    // 复制构造
    matrix(matrix const &m) :
            data(new T[m.n_col * m.n_row]),
            n_col(m.n_col),
            n_row(m.n_row) {
        std::copy(m.begin(), m.end(), begin());
    }

    template<typename Iterator>
    matrix(size_t n_row, size_t n_col, Iterator i):
            data(new T[n_row * n_col]), n_row(n_row), n_col(n_col) {
        std::copy_n(i, n_col * n_row, begin());
    }

    // 析构
    ~matrix() { delete data; }

    iterator begin() { return data; }

    iterator end() {
        return data + n_row * n_col;
    }

    const iterator begin() const { return data; }

    const iterator end() const {
        return data + n_row * n_col;
    }

    row_iterator row_begin(size_t n) { return data + n * n_col; }

    row_iterator row_end(size_t n) { return row_begin(n) + n_col; }

    const row_iterator row_begin(size_t n) const { return data + n * n_col; }

    const row_iterator row_end(size_t n) const { return row_begin(n) + n_col; }

    col_iterator col_begin(size_t n) { return col_iterator(data + n, n_col); }

    // 此处列都是使用的 col_iterator 重载后的 +
    col_iterator col_end(size_t n) { return col_begin(n) + n_row; }

    const col_iterator col_begin(size_t n) const { return col_iterator(data + n, n_col); }

    // 此处列都是使用的 col_iterator 重载后的 +
    const col_iterator col_end(size_t n) const { return col_begin(n) + n_row; }

    size_t num_row() const { return n_row; }

    size_t num_col() const { return n_col; }

    T &operator()(size_t i, size_t j) { return data[i + n_col + j]; }

    T const &operator()(size_t i, size_t j) const { return data[i + n_col + j]; }

    // 赋值构造
    matrix &operator=(matrix const &m) {
        if (&m == this) return *this;
        if (n_row * n_col < m.n_row * m.n_col) {
            delete[] data; // array delete
            data = new T[m.n_col * m.n_row];
        }
        n_col = m.n_col;
        n_row = m.n_row;

        std::copy(m.begin(), m.end(), begin());
    }
};


template<typename T>
matrix<T> operator*(matrix<T> const &m0, matrix<T> const &m1) noexcept(false) {
    if (m0.num_col() != m1.num_row()) {
        throw std::runtime_error("Bad matrix size for multiplication.");
    }
    matrix<T> m(m0.num_row(), m1.num_col());

    typename matrix<T>::iterator pos = m.begin();
    for (size_t i = 0; i < m.num_row(); i++) {
        for (size_t j = 0; j < m.num_col(); ++j) {
            *pos = 0;
            std::transform(m0.row_begin(i), m0.row_end(i),
                           m1.col_begin(j),
                           accumulater(*pos, std::plus<T>()),
                           std::multiplies<T>());
            ++pos;
        }
    }
    return m;
}
