//
// Created by andrey on 25.05.18.
//

#ifndef VECTOR_DIGIT_VECTOR_DIGIT_H
#define VECTOR_DIGIT_VECTOR_DIGIT_H


#include <cstdint>
#include <cstddef>
#include <memory>


const size_t SMALL_SIZE = 8;
const bool MINUS = true;
const bool PLUS = false;


class vector_digit {
    typedef uint64_t digit;

    struct dynamic_storage {
        struct Deleter {
            void operator()(digit *ptr) {
                free(ptr);
            }
        } deleter;

        size_t capacity = 0;
        std::shared_ptr<digit> ptr = nullptr;

        dynamic_storage() = default;

        explicit dynamic_storage(size_t capacity);

        dynamic_storage(dynamic_storage const &other);

        dynamic_storage &operator=(dynamic_storage const &other);

        ~dynamic_storage();

    };

    struct inplace_storage {
        digit data[SMALL_SIZE];

        inplace_storage() = default;

        inplace_storage(inplace_storage const &other) {
            *this = other;
        }

        inplace_storage &operator=(inplace_storage const &other){
            std::copy(other.data, other.data + SMALL_SIZE, data);
            return *this;
        }
    };

    union both_storage {
        dynamic_storage dynamic;
        inplace_storage inplace;

        both_storage() {
            inplace = inplace_storage();
            std::fill(inplace.data, inplace.data + SMALL_SIZE, 0);
        }

        ~both_storage() {

        };
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

    explicit vector_digit(size_t size);

    vector_digit(size_t size, digit default_value);

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

    digit &at(size_t index);

    digit unbound_get(size_t index) const;

    digit unchecked_get(size_t index) const;

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
    return size_t(is_small()) * SMALL_SIZE
           + size_t(!is_small()) * storage.dynamic.capacity;
}

inline vector_digit::digit &vector_digit::at(size_t index) {
    return begin()[index];
}

inline vector_digit::digit vector_digit::unbound_get(size_t index) const {
    /*if (index >= size()){
        return leading();
    }
    else{
        return cur_data_pointer[index];
    }*/
    bool out_of_range = index >= size();
    index &= capacity() - 1;
    return cur_data_pointer[index] * static_cast<size_t>(!out_of_range)
           + leading() * static_cast<size_t>(out_of_range);
}

inline vector_digit::digit vector_digit::leading() const {
    return is_negative() ? ~0ull : 0ull;
}


inline const vector_digit::digit *vector_digit::begin() const {
    return cur_data_pointer;
}


inline vector_digit::digit *vector_digit::begin() {
   // prepare_change();
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

inline vector_digit::digit vector_digit::unchecked_get(size_t index) const {
    return cur_data_pointer[index];
}

inline vector_digit::digit &vector_digit::operator[](size_t index) {
    return at(index);
}

inline vector_digit::digit vector_digit::operator[](size_t index) const {
    return unchecked_get(index);
}

#endif //VECTOR_DIGIT_VECTOR_DIGIT_H
