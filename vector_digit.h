//
// Created by andrey on 25.05.18.
//
#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>


class vector_digit {
    static const size_t SMALL_SIZE = 4;
public:
    static const bool MINUS = true;
    static const bool PLUS = false;
private:
    typedef uint64_t digit;

    struct dynamic_storage {
        size_t capacity = 0;
        digit *ptr = nullptr;

        dynamic_storage &operator=(dynamic_storage const &other) {
            connect(other.ptr);
            capacity = other.capacity;
            return *this;
        }

        dynamic_storage() = default;

        dynamic_storage(size_t capacity)
                : capacity(capacity) {
            auto *mem = new digit[capacity + 1];
            *mem = 0;
            connect(mem + 1);
        }

        digit &ref_counter(){
            return ptr[-1];
        }

        void disconnect() {
            if (ptr != nullptr) {
                ref_counter()--;
                if (ref_counter() == 0) {
                    delete[] (ptr - 1);
                }
            }
        }

        void connect(digit *new_ptr) {
            disconnect();
            ptr = new_ptr;
            ref_counter()++;
        }

        bool unique() {
            return ref_counter() == 1;
        }

        ~dynamic_storage() {
            disconnect();
        }
    };

    struct inplace_storage {
        digit data[SMALL_SIZE];

        inplace_storage &operator=(inplace_storage const &other) {
            std::copy(other.data, other.data + SMALL_SIZE, data);
            return *this;
        }
    };

    union both_storage {
        dynamic_storage dynamic;
        inplace_storage inplace;

        both_storage() {}

        ~both_storage() {}
    } storage;


    void move_memory(size_t new_capacity);
    void prepare_change();
    void reset_to_small(inplace_storage const &other);
    void reset_to_big(dynamic_storage const &other);
    bool is_small() const;
    void fix_capacity();

public:
    vector_digit();
    vector_digit(vector_digit const &other);
    vector_digit &operator=(vector_digit const &other);
    ~vector_digit();

    void push_back(digit d);
    void pop_back();
    void resize(size_t size);
    void resize(size_t new_size, digit default_value);

    digit back() const;
    bool is_negative() const;
    void set_sign(bool new_sign);


    /* frequently used methods placed below for inlining
     * ~750 ms (without inline) --> ~600 ms */

    digit unbound_get(size_t index) const {
        return index >= size() ? leading() : cur_data_pointer[index];
    }

    digit &operator[](size_t index) {
        return begin()[index];
    }

    digit operator[](size_t index) const {
        return cur_data_pointer[index];
    }

    size_t size() const {
        return _size;
    }

    size_t capacity() const {
        if (is_small()) {
            return SMALL_SIZE;
        } else {
            return storage.dynamic.capacity;
        }
    }

    const digit *cbegin() const {
        return cur_data_pointer;
    }

    digit *begin() {
        prepare_change();
        return cur_data_pointer;
    }

    const digit *cend() const {
        return cbegin() + size();
    }

    digit *end() {
        return begin() + size();
    }

    digit leading() const {
        static const digit vars[2] = {0ull, ~0ull};
        return vars[is_negative()];
    }

private:

    digit *cur_data_pointer;
    size_t _size;
    bool sign;
};

