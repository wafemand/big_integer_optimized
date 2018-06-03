#include <algorithm>
#include "big_integer.h"


typedef uint64_t digit;


big_integer::big_integer() {
    digits.push_back(0);
    digits.set_sign(PLUS);
}


big_integer::big_integer(int a) {
    digits.push_back(static_cast<digit>(a));
    digits.set_sign(a < 0 ? MINUS : PLUS);
}


big_integer::big_integer(uint64_t a) {
    digits.push_back(a);
    digits.set_sign(PLUS);
}


big_integer::big_integer(std::string const &str) : big_integer()
{
    bool is_neg = str[0] == '-';
    size_t cur_dig_size = str.size() % READ_BLOCK_SIZE;
    if (is_neg) {
        cur_dig_size--;
    }
    if (cur_dig_size == 0) {
        cur_dig_size = READ_BLOCK_SIZE;
    }
    for (auto i = static_cast<size_t>(is_neg); i < str.size();) {
        *this *= READ_BLOCK_BASE;
        *this += static_cast<uint64_t>(std::stoll(str.substr(i, cur_dig_size)));
        i += cur_dig_size;
        cur_dig_size = READ_BLOCK_SIZE;
    }
    if (is_neg) {
        negate();
    }
}


big_integer &big_integer::operator+=(big_integer const &rhs) {
    add(rhs);
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    return *this += -rhs;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    mul(rhs);
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    div_mod(rhs);
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    bitwise<std::bit_and<digit> >(rhs);
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    bitwise<std::bit_or<digit>>(rhs);
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    bitwise<std::bit_xor<digit>>(rhs);
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    *this = div_mod(rhs);
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    shift_right(rhs);
    return *this;
}

big_integer &big_integer::operator<<=(int rhs) {
    shift_left(rhs);
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer res = *this;
    res.negate();
    return res;
}

big_integer big_integer::operator~() const {
    big_integer res = *this;
    res.bit_negate();
    return res;
}

big_integer &big_integer::operator++() {
    return *this;
}

big_integer &big_integer::operator--() {
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer res = *this;
    add(1);
    return res;
}

big_integer big_integer::operator--(int) {
    big_integer res = *this;
    add(-1);
    return res;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}


bool operator==(big_integer const &a, big_integer const &b) {
    return cmp(a, b) == 0;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return cmp(a, b) != 0;
}

bool operator<(big_integer const &a, big_integer const &b) {
    return cmp(a, b) == -1;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return cmp(a, b) == 1;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    int cmp_res = cmp(a, b);
    return cmp_res == -1 || cmp_res == 0;
}

bool operator>=(big_integer const &a, big_integer const &b) {
    int cmp_res = cmp(a, b);
    return cmp_res == 1 || cmp_res == 0;
}


std::string to_string(big_integer const &a) {
    std::string res;
    big_integer q = a.abs();
    do {
        digit dig = (q % 10).digits[0];
        res.push_back('0' + static_cast<char>(dig));
        q /= 10;
    } while (q > 0);
    if (a.digits.is_negative()){
        res.push_back('-');
    }
    std::reverse(res.begin(), res.end());
    return res;
}


std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s;
}
