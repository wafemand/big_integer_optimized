#include <iostream>
#include "big_integer.h"
#include "vector_digit.h"
#include "gtest/gtest.h"

using namespace std;


void multipush_1_n(vector_digit &v, size_t n){
    for (size_t i = 1; i <= n; ++i) {
        v.push_back(i);
    }
}


TEST(vector_test, leading_digit){
    vector_digit v;
    vector_digit const &u = v;

    multipush_1_n(v, 4);
    // small object
    EXPECT_EQ(0ull, u[10]);
    v.set_sign(MINUS);
    EXPECT_EQ(~0ull, u[10]);

    multipush_1_n(v, 2);
    // big object
    EXPECT_EQ(~0ull, u[7]);
    EXPECT_EQ(~0ull, u[10]);
    v.set_sign(PLUS);
    EXPECT_EQ(0ull, u[10]);

    multipush_1_n(v, 4);

    EXPECT_EQ(3, u[8]);
    v.set_sign(MINUS);
    EXPECT_EQ(3, u[8]);
}


TEST(vector_test, copy_on_write){
    vector_digit v;
    multipush_1_n(v, 10);
    vector_digit u1 = v;
    vector_digit u2 = v;
    vector_digit const &const_v = v;
    vector_digit const &const_u1 = u1;
    vector_digit const &const_u2 = u2;

    EXPECT_EQ(const_u1.begin(), const_u2.begin());
    EXPECT_EQ(const_v.begin(), const_u2.begin());

    u1.push_back(123);

    EXPECT_NE(const_u1.begin(), const_u2.begin());
    EXPECT_EQ(const_v.begin(), const_u2.begin());
    EXPECT_EQ(123, u1[u1.size() - 1]);

    v.push_back(123);

    EXPECT_NE(const_u1.begin(), const_u2.begin());
    EXPECT_NE(const_v.begin(), const_u1.begin());
    EXPECT_NE(const_v.begin(), const_u2.begin());
    EXPECT_EQ(123, v[v.size() - 1]);
}