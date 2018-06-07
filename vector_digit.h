//
// Created by andrey on 25.05.18.
//

#ifndef VECTOR_DIGIT_VECTOR_DIGIT_H
#define VECTOR_DIGIT_VECTOR_DIGIT_H


#include <cstdint>
#include <cstddef>


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
        }

        dynamic_storage() = default;

        dynamic_storage(size_t capacity)
            : capacity(capacity)
        {
            auto *mem = static_cast<digit *>(malloc((capacity + 1) * sizeof(digit)));
            *mem = 0;
            connect(mem + 1);
        }

        void disconnect(){
            if (ptr != nullptr){
                ptr[-1]--;
                if (ptr[-1] == 0){
                    free(ptr - 1);
                }
            }
        }

        void connect(digit *new_ptr){
            disconnect();
            ptr = new_ptr;
            ptr[-1]++;
        }

        bool unique(){
            return ptr[-1] == 1;
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

    digit &operator[](size_t index);

    digit operator[](size_t index) const;

    void push_back(digit d);

    void pop_back();

    void resize(size_t size);

    void resize(size_t new_size, digit default_value);

    size_t size() const;

    size_t capacity() const;

    digit unbound_get(size_t index) const;

    digit leading() const;

    const digit *begin() const;

    digit *begin();

    const digit *end() const;

    digit *end();

    digit back() const;

    bool is_negative() const;

    void set_sign(bool new_sign);

    static void get_counts();
};

inline bool vector_digit::is_small() const {
    return cur_data_pointer == storage.inplace.data;
}

inline void vector_digit::resize(size_t size) {
    resize(size, leading());
}

inline size_t vector_digit::size() const {
    return _size;
}

inline size_t vector_digit::capacity() const {
    const digit vars[2] = {storage.dynamic.capacity, SMALL_SIZE};
    return vars[is_small()];
}


inline vector_digit::digit vector_digit::unbound_get(size_t index) const {
    return index >= size() ? leading() : cur_data_pointer[index];
}

inline vector_digit::digit vector_digit::leading() const {
    static const digit vars[2] = {0ull, ~0ull};
    return vars[is_negative()];
}


inline const vector_digit::digit *vector_digit::begin() const {
    return cur_data_pointer;
}


inline vector_digit::digit *vector_digit::begin() {
    prepare_change();
    return cur_data_pointer;
}

inline const vector_digit::digit *vector_digit::end() const {
    return begin() + size();
}

inline vector_digit::digit *vector_digit::end() {
    return begin() + size();
}

inline bool vector_digit::is_negative() const {
    return sign;
}

inline void vector_digit::set_sign(bool new_sign) {
    sign = new_sign;
}

inline vector_digit::digit vector_digit::back() const {
    return unbound_get(size() - 1);
}

inline vector_digit::digit &vector_digit::operator[](size_t index) {
    return begin()[index];
}

inline vector_digit::digit vector_digit::operator[](size_t index) const {
    return cur_data_pointer[index];
}

inline void vector_digit::prepare_change() {
    if (!is_small() && !storage.dynamic.unique()) {
        move_memory(storage.dynamic.capacity);
    }
}

#endif //VECTOR_DIGIT_VECTOR_DIGIT_H
