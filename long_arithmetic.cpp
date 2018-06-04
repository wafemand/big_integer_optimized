//
// Created by andrey on 11.04.18.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstring>
#include "big_integer.h"


void big_integer::subtract(big_integer const &rhs) {
    size_t max_size = std::max(digits.size(), rhs.digits.size()) + 1;
    size_t rhs_size = rhs.digits.size();
    digits.resize(max_size, digits.leading());
    digit carry = 1;
    digit *this_ptr = digits.begin();

    for (size_t i = 0; i < rhs_size; ++i) {
        digit cur_rhs = ~rhs.digits[i];
        carry = add_with_overflow(this_ptr[i], carry);
        carry += add_with_overflow(this_ptr[i], cur_rhs);
    }
    digit lead = ~rhs.digits.leading();

    for (size_t i = rhs_size; i < max_size; i++) {
        carry = add_with_overflow(this_ptr[i], carry);
        carry += add_with_overflow(this_ptr[i], lead);
    }

    digits.set_sign(leading_bit(digits.back()));

    pop_zeros();
}


void big_integer::add(big_integer const &rhs) {
    size_t max_size = std::max(digits.size(), rhs.digits.size()) + 1;
    size_t rhs_size = rhs.digits.size();
    digits.resize(max_size, digits.leading());
    digit carry = 0;
    digit *data_ptr = digits.begin();

    for (size_t i = 0; i < rhs_size; ++i) {
        digit cur_rhs = rhs.digits[i];
        carry = add_with_overflow(data_ptr[i], carry);
        carry += add_with_overflow(data_ptr[i], cur_rhs);
    }
    digit lead = rhs.digits.leading();

    for (size_t i = rhs_size; i < max_size; i++) {
        carry = add_with_overflow(data_ptr[i], carry);
        carry += add_with_overflow(data_ptr[i], lead);
    }

    digits.set_sign(leading_bit(digits.back()));

    pop_zeros();
}


int cmp(big_integer const &a, big_integer const &b) { // (a - b) = res * |a - b|
    if (a.digits.is_negative() != b.digits.is_negative()) {
        return int(b.digits.is_negative()) - int(a.digits.is_negative());
    }
    if (a.digits.size() != b.digits.size()) {
        if (a.digits.is_negative()) {
            return a.digits.size() < b.digits.size() ? 1 : -1;
        } else {
            return a.digits.size() < b.digits.size() ? -1 : 1;
        }
    }
    size_t cur_size = a.digits.size();
    for (size_t j = 0; j < cur_size; j++) {
        size_t i = cur_size - j - 1;
        if (a.digits[i] != b.digits[i]) {
            if (a.digits[i] < b.digits[i]) {
                return -1;
            } else {
                return 1;
            }
        }
    }
    return 0;
}


void big_integer::bit_negate() {
    digit *this_ptr = digits.begin();
    for (size_t i = 0; i < digits.size(); i++) {
        this_ptr[i] = ~this_ptr[i];
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
    digit *this_ptr = digits.begin();
    for (size_t i = 0; i < digits.size(); i++) {
        digit cur = carry;
        carry = 0;
        carry += mul_with_overflow(this_ptr[i], rhs);
        carry += add_with_overflow(this_ptr[i], cur);
    }
    digits.push_back(carry);
    pop_zeros();
}


void big_integer::unsigned_mul(big_integer const &rhs) {
    const big_integer a = *this;
    const big_integer &b = rhs;
    *this = 0;
    digits.resize(a.digits.size() + b.digits.size() + 1);
    digit *this_ptr = digits.begin();
    for (size_t i = 0; i < a.digits.size(); i++) {
        digit cur_a = a.digits[i];
        digit carry = 0;
        for (size_t j = 0; j < b.digits.size(); j++) {
            digit cur_b = b.digits[j];
            carry = add_with_overflow(this_ptr[i + j], carry);
            carry += mul_with_overflow(cur_b, cur_a);
            carry += add_with_overflow(this_ptr[i + j], cur_b);
        }
        this_ptr[i + b.digits.size()] = carry;
    }
    pop_zeros();
}


digit estimate(digit r0, digit r1, const digit d) {
    __uint128_t r2 = r1 + (static_cast<__uint128_t>(r0) << BITS);
    return (digit) std::min(r2 / d, static_cast<__uint128_t>(~0ull));
}


big_integer big_integer::unsigned_div_mod(digit rhs) {
    digit carry = 0;
    digit *this_ptr = digits.begin();
    for (int64_t i = digits.size() - 1; i >= 0; i--) {
        __uint128_t cur = this_ptr[i] + (static_cast<__uint128_t>(carry) << BITS);
        this_ptr[i] = digit(cur / rhs);
        carry = digit(cur % rhs);
    }
    pop_zeros();
    return carry;
}


big_integer big_integer::unsigned_div_mod(big_integer const &rhs) {
    big_integer a = *this;
    big_integer b = rhs;
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
    digit scale = (~digit(0)) / (rhs.digits.back() + 1);
    a.unsigned_mul(scale);
    b.unsigned_mul(scale);

    auto n = static_cast<int64_t>(a.digits.size());
    auto m = static_cast<int64_t>(b.digits.size());
    digits.resize(static_cast<size_t>(n - m + 1));
    digit *ptr_a = a.digits.begin();
    digit *this_ptr = digits.begin();
    digit b_lead = b.digits[m - 1];
    big_integer rem = 0;

    rem.digits.resize(static_cast<size_t>(m + 1));
    std::copy(ptr_a + n - m + 1, ptr_a + n, rem.digits.begin());

    for (int64_t i = n - m; i >= 0; i--) {
        rem <<= BITS;
        rem.digits[0] = ptr_a[i];
        digit est = estimate(
                rem.digits.unbound_get(static_cast<size_t>(m)),
                rem.digits.unbound_get(static_cast<size_t>(m - 1)),
                b_lead);
        big_integer tmp = b;
        tmp.unsigned_mul(est);
        while (rem < tmp) {
            tmp -= b;
            est--;
        }
        rem -= tmp;
        assert(rem < b);
        *this_ptr = est;
        this_ptr++;
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
    digit *this_ptr = digits.begin();
    if (digit_shift > 0) {
        for (int64_t i = n - 1; i >= digit_shift; i--) {
            this_ptr[i] = this_ptr[i - digit_shift];
        }
        for (int64_t i = digit_shift - 1; i >= 0; i--) {
            this_ptr[i] = 0;
        }
    }
    if (bit_shift > 0) {
        for (int64_t i = n - 1; i > 0; i--) {
            this_ptr[i] <<= bit_shift;
            this_ptr[i] |= this_ptr[i - 1] >> (BITS - bit_shift);
        }
        this_ptr[0] <<= bit_shift;
    }
    pop_zeros();
}


void big_integer::shift_right(int shift) {
    size_t bit_shift = shift % BITS;
    size_t digit_shift = shift / BITS;
    if (digit_shift > 0) {
        digit *this_ptr = digits.begin();
        for (size_t i = 0; i < digits.size() - digit_shift; i++) {
            this_ptr[i] = this_ptr[i + digit_shift];
        }
    }
    digits.resize(digits.size() - digit_shift + 1);
    if (bit_shift > 0) {
        digit *this_ptr = digits.begin();
        for (size_t i = 0; i < digits.size() - 1; i++) {
            this_ptr[i] >>= bit_shift;
            this_ptr[i] |= this_ptr[i + 1] << (BITS - bit_shift);
        }
    }
    digits.pop_back();
    pop_zeros();
}

