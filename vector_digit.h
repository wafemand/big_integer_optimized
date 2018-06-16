//
// Created by andrey on 25.05.18.
//

#ifndef VECTOR_DIGIT_VECTOR_DIGIT_H
#define VECTOR_DIGIT_VECTOR_DIGIT_H


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
            // А где return?
        }

        dynamic_storage() = default;

        dynamic_storage(size_t capacity)
                : capacity(capacity) {
            // не используй Сишные функции
            // operator new(...) ~ malloc
            // new digit[capacity + 1]
            auto *mem = static_cast<digit *>(malloc((capacity + 1) * sizeof(digit)));
            *mem = 0;
            connect(mem + 1);
        }

        void disconnect() {
            if (ptr != nullptr) {
                ptr[-1]--;
                if (ptr[-1] == 0) {
                    free(ptr - 1);
                }
            }
        }

        void connect(digit *new_ptr) {
            disconnect();
            ptr = new_ptr;
            // лучше сделать метод вида, get_ptr_to_counter
            ptr[-1]++;
        }

        bool unique() {
            return ptr[-1] == 1;
        }

        ~dynamic_storage() {
            disconnect();
        }
    };

    struct inplace_storage {
        // std::array
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

    // старайся не писать определения членов в середине класса
    // либо в начале, либо в конце
    digit *cur_data_pointer;
    size_t _size;
    bool sign;

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


    /* often use methods for inline
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

    // cbegin бывает удобнее, но не суть
    const digit *begin() const {
        return cur_data_pointer;
    }

    digit *begin() {
        prepare_change();
        return cur_data_pointer;
    }

    const digit *end() const {
        return begin() + size();
    }

    digit *end() {
        return begin() + size();
    }

    digit leading() const {
        static const digit vars[2] = {0ull, ~0ull};
        return vars[is_negative()];
    }
};


#endif //VECTOR_DIGIT_VECTOR_DIGIT_H
