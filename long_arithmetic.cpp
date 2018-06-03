//
// Created by andrey on 11.04.18.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include "big_integer.h"


void big_integer::add(big_integer const &rhs) {
    size_t ans_size = std::max(digits.size(), rhs.digits.size()) + 1;
    digits.resize(ans_size, digits.leading());
    digit carry = 0;
    for (size_t i = 0; i < ans_size; ++i) {
        digit cur_rhs = rhs.digits.unbound_get(i);
        carry = add_with_overflow(digits[i], carry);
        carry += add_with_overflow(digits[i], cur_rhs);
    }

    digits.set_sign(leading_bit(digits.back()));

    pop_zeros();
}


int cmp(big_integer const &a, big_integer const &b) { // (a - b) = res * |a - b|
    if (a.digits.is_negative() != b.digits.is_negative()) {
        return int(b.digits.is_negative()) - int(a.digits.is_negative());
    }
    size_t max_size = std::max(a.digits.size(), b.digits.size());
    for (size_t j = 0; j < max_size; j++) {
        size_t i = max_size - j - 1;
        if (a.digits.unbound_get(i) != b.digits.unbound_get(i)) {
            if (a.digits.unbound_get(i) < b.digits.unbound_get(i)) {
                return -1;
            } else {
                return 1;
            }
        }
    }
    return 0;
}


void big_integer::bit_negate() {
    for (size_t i = 0; i < digits.size(); i++) {
        digits[i] = ~digits[i];
    }
    digits.set_sign(!digits.is_negative());
}


void big_integer::negate() {
    bit_negate();
    add(1);
}


big_integer big_integer::abs() const {
    if (digits.is_negative()) {
        return -*this;
    } else {
        return *this;
    }
}


void big_integer::unsigned_mul(digit rhs) {
    digit carry = 0;
    for (size_t i = 0; i < digits.size(); i++){
        carry = add_with_overflow(digits[i], carry);
        carry += mul_with_overflow(digits[i], rhs);
    }
    digits.push_back(carry);
    pop_zeros();
}


void big_integer::unsigned_mul(big_integer const &rhs) {
    const big_integer a = *this;
    const big_integer &b = rhs;
    *this = 0;
    digits.resize(a.digits.size() + b.digits.size() + 1);
    for (size_t i = 0; i < a.digits.size(); i++) {
        digit cur_a = a.digits[i];
        digit carry = 0;
        for (size_t j = 0; j < b.digits.size(); j++) {
            digit cur_b = b.digits[j];
            carry = add_with_overflow(digits[i + j], carry);
            carry += mul_with_overflow(cur_b, cur_a);
            carry += add_with_overflow(digits[i + j], cur_b);
        }
        digits[i + b.digits.size()] = carry;
    }
    pop_zeros();
}


digit estimate(digit r0, digit r1, const digit d) {
    static const digit HALF = BITS / 2;
    __uint128_t r2 = r1 + (static_cast<__uint128_t>(r0) << BITS);
    return (digit) std::min(r2 / d, static_cast<__uint128_t>(~0ull));
}


big_integer big_integer::unsigned_div_mod(digit rhs) {
    big_integer const a = *this;
    big_integer const b = rhs;
    digits.resize(0);
    big_integer rem = 0;
    for (int64_t i = a.digits.size() - 1; i >= 0; i--) {
        rem <<= BITS;
        rem += a.digits[i];
        digit est = estimate(rem.digits.unbound_get(1), rem.digits.unbound_get(0), rhs);
        big_integer tmp = b;
        tmp.unsigned_mul(est);
        rem -= tmp;
        digits.push_back(est);
    }
    std::reverse(digits.begin(), digits.end());
    pop_zeros();
    return rem;
}


big_integer big_integer::unsigned_div_mod(big_integer const &rhs) {
    digit scale = (~digit(0)) / (rhs.digits.back() + 1);
    big_integer const a = *this * scale;
    big_integer const b = rhs * scale;
    if (b == 0) {
        throw std::exception();
        // todo
    }
    if (a < b) {
        big_integer const rem = *this;
        *this = 0;
        return rem;
    }
    if (a == 0) {
        *this = 0;
        return rhs;
    }
    auto n = static_cast<int64_t>(a.digits.size());
    auto m = static_cast<int64_t>(b.digits.size());
    big_integer rem = 0;
    rem.digits.pop_back();
    for (int64_t i = n - m + 1; i < n; i++) {
        rem.digits.push_back(a.digits[i]);
    }
    digits.resize(0);
    for (int64_t i = n - m; i >= 0; i--) {
        rem <<= BITS;
        rem += a.digits[i];
        digit est = estimate(
                rem.digits.unbound_get(m),
                rem.digits.unbound_get(m - 1),
                b.digits[m - 1]);
        big_integer tmp = b;
        tmp.unsigned_mul(est);
        while (rem < tmp) {
            tmp -= b;
            est--;
        }
        rem -= tmp;

        assert(rem < b);
        digits.push_back(est);
    }
    std::reverse(digits.begin(), digits.end());
    rem.unsigned_div_mod(scale);
    pop_zeros();
    rem.pop_zeros();
    return rem;
}


void big_integer::shift_left(int shift) {
    int64_t bit_shift = shift % BITS;
    int64_t digit_shift = shift / BITS;
    digits.resize(digits.size() + digit_shift + 1);
    size_t n = digits.size();
    if (digit_shift > 0) {
        for (int64_t i = n - 1; i >= digit_shift; i--) {
            digits[i] = digits[i - digit_shift];
        }
        for (int64_t i = digit_shift - 1; i >= 0; i--) {
            digits[i] = 0;
        }
    }
    if (bit_shift > 0) {
        for (int64_t i = n - 1; i > 0; i--) {
            digits[i] <<= bit_shift;
            digits[i] |= digits[i - 1] >> (BITS - bit_shift);
        }
        digits[0] <<= bit_shift;
    }
    pop_zeros();
}


void big_integer::shift_right(int shift) {
    size_t bit_shift = shift % BITS;
    size_t digit_shift = shift / BITS;
    for (size_t i = 0; i < digits.size() - digit_shift; i++) {
        digits[i] = digits[i + digit_shift];
    }
    digits.resize(digits.size() - digit_shift + 1);
    for (size_t i = 0; i < digits.size() - 1; i++) {
        digits[i] >>= bit_shift;
        digits[i] |= digits[i + 1] << (BITS - bit_shift);
    }
    digits.pop_back();
    pop_zeros();
}

