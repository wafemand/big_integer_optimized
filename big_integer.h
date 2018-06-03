//
// Created by andrey on 11.04.18.
//
#pragma once

#ifndef BIG_INTEGER_OPTIMIZED_BIG_INTEGER_H
#define BIG_INTEGER_OPTIMIZED_BIG_INTEGER_H


#include <cstdint>
#include <string>
#include "utils.h"
#include "vector_digit.h"

const digit BITS = sizeof(digit) * 8;

class big_integer {

    typedef uint64_t digit;

public:
    big_integer();
    big_integer(uint64_t a);
    big_integer(big_integer const& other) = default;
    big_integer(int a);
    explicit big_integer(std::string const& str);
    ~big_integer() = default;

    big_integer& operator=(big_integer const& other) = default;

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);


private:
    vector_digit digits;

    void add(big_integer const &rhs);

    void mul(big_integer const &rhs);

    big_integer abs() const;

    big_integer div_mod(big_integer const &rhs);

    big_integer div_mod(digit rhs);

    void shift_left(int shift);

    void shift_right(int shift);

    template <class Op>
    void bitwise(big_integer const &rhs);

    void bit_negate();

    void negate();

    void pop_zeros();

    friend int cmp(big_integer const &a, big_integer const &b);
};



big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const &a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);


template<typename Op>
inline void big_integer::bitwise(big_integer const &rhs) {
    Op op;
    size_t ans_size = std::max(digits.size(), rhs.digits.size());
    digits.resize(ans_size, digits.leading());
    for (size_t i = 0; i < ans_size; ++i) {
        digits[i] = op(digits[i], rhs.digits[i]);
    }
    digits.set_sign(leading_bit(digits.back()));
    pop_zeros();
}


inline void big_integer::pop_zeros() {
    size_t cur = digits.size();
    while (cur > 1 && digits.unchecked_get(cur - 1) == digits.leading()) {
        cur--;
    }
    digits.resize(cur);
}


#endif //BIG_INTEGER_OPTIMIZED_BIG_INTEGER_H
