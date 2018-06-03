//
// Created by andrey on 11.04.18.
//
#pragma once

#ifndef BIG_INTEGER_OPTIMIZED_UTILS_H
#define BIG_INTEGER_OPTIMIZED_UTILS_H


#include <cstdint>
#include <cstddef>

typedef uint64_t digit;

static const int READ_BLOCK_SIZE = 15;
static const uint64_t READ_BLOCK_BASE = 1000ull * 1000ull * 1000ull * 1000ull * 1000ull;


template<typename T>
inline T add_with_overflow(T &a, T b) {
    T c = a;
    a += b;
    if (c > a) {
        return 1;
    } else {
        return 0;
    }
}

inline digit mul_with_overflow(digit &first, digit second) {
    static const int BITS = sizeof(digit) * 8;
    __uint128_t a128 = first;
    __uint128_t b128 = second;
    a128 *= b128;
    first = digit(a128);
    return digit(a128 >> BITS);
    int half = BITS / 2;
    digit a = first >> half;
    digit b = (first << half) >> half;
    digit c = second >> half;
    digit d = (second << half) >> half;
    digit tmp = b * c;
    digit carry = add_with_overflow(tmp, a * d);
    digit h = ((b * c + a * d) >> half) + (carry << half);
    digit l = (b * c + a * d) << half;
    first *= second;
    return a * c + h + add_with_overflow(l, b * d);
}


template <typename T>
inline bool leading_bit(T val){
    return val >> (sizeof(T) * 8 - 1);
}


#endif //BIG_INTEGER_OPTIMIZED_UTILS_H
