//
// Created by andrey on 25.05.18.
//

#include <algorithm>
#include <cassert>
#include <iostream>
#include "vector_digit.h"


typedef uint64_t digit;


size_t next_pow_2(size_t x) {
    while ((x & (x - 1)) != 0) {
        x &= (x - 1);
    }
    return x * 2;
}

void vector_digit::move_memory(size_t new_capacity) {
    assert(new_capacity >= SMALL_SIZE);
    auto new_storage = dynamic_storage(new_capacity);

    std::copy(cur_data_pointer, cur_data_pointer + std::min(size(), new_capacity), new_storage.ptr);
    reset_to_big(new_storage);
}

void vector_digit::prepare_change() {
    if (!is_small() && !storage.dynamic.unique()) {
        move_memory(storage.dynamic.capacity);
    }
}

void vector_digit::reset_to_small(inplace_storage const &other) {
    if (!is_small()) {
        storage.dynamic.~dynamic_storage();
    }
    storage.inplace = other;
    cur_data_pointer = storage.inplace.data;
}

void vector_digit::reset_to_big(dynamic_storage const &other) {
    if (is_small()) {
        new(&storage.dynamic) dynamic_storage();
    }
    storage.dynamic = other;
    cur_data_pointer = storage.dynamic.ptr;
}

bool vector_digit::is_small() const {
    return cur_data_pointer == storage.inplace.data;
}

void vector_digit::fix_capacity() {
    if (size() >= capacity()) {
        move_memory(2 * capacity());
    } else if (size() < capacity() / 4 && capacity() > SMALL_SIZE) {
        move_memory(capacity() / 2);
    }
}


// используй список инициализаций
vector_digit::vector_digit() {
    sign = PLUS;
    cur_data_pointer = storage.inplace.data;
    this->_size = 0;
}

vector_digit::vector_digit(vector_digit const &other) : vector_digit() {
    *this = other;
}

vector_digit &vector_digit::operator=(vector_digit const &other) {
    if (other.is_small()) {
        reset_to_small(other.storage.inplace);
    } else {
        reset_to_big(other.storage.dynamic);
    }
    sign = other.sign;
    _size = other._size;
    return *this;
}

vector_digit::~vector_digit() {
    if (!is_small()) {
        storage.dynamic.~dynamic_storage();
    }
}


void vector_digit::push_back(digit d) {
    fix_capacity();
    _size++;
    begin()[_size - 1] = d;
}

void vector_digit::pop_back() {
    fix_capacity();
    _size--;
}

void vector_digit::resize(size_t size) {
    resize(size, leading());
}

void vector_digit::resize(size_t new_size, digit default_value) {
    if (new_size > SMALL_SIZE) {
        move_memory(next_pow_2(new_size));
    }
    size_t old_size = _size;
    _size = new_size;
    if (old_size < _size) {
        prepare_change();
        while (old_size < _size) {
            cur_data_pointer[old_size] = default_value;
            old_size++;
        }
    }
}

vector_digit::digit vector_digit::back() const {
    return *(end() - 1);
}

bool vector_digit::is_negative() const {
    return sign;
}

void vector_digit::set_sign(bool new_sign) {
    sign = new_sign;
}
