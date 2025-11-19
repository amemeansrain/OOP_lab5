#pragma once
#include <iterator>

// Forward declaration для forward_list
template <typename T>
class forward_list;

template <typename T>
class forward_list_iterator {
private:
    // Используем публичный тип из forward_list
    typename forward_list<T>::Node* current_;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    // Конструктор должен быть дружественным
    explicit forward_list_iterator(typename forward_list<T>::Node* node = nullptr) 
        : current_(node) {}

    reference operator*() const { 
        return current_->value; 
    }

    pointer operator->() const { 
        return &current_->value; 
    }

    forward_list_iterator& operator++() {
        if (current_) {
            current_ = current_->next;
        }
        return *this;
    }

    forward_list_iterator operator++(int) {
        forward_list_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const forward_list_iterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const forward_list_iterator& other) const {
        return current_ != other.current_;
    }

    // Для доступа к текущему узлу
    typename forward_list<T>::Node* node() const {
        return current_;
    }
};

template <typename T>
class forward_list_const_iterator {
private:
    const typename forward_list<T>::Node* current_;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    explicit forward_list_const_iterator(const typename forward_list<T>::Node* node = nullptr) 
        : current_(node) {}
    
    forward_list_const_iterator(const forward_list_iterator<T>& other)
        : current_(other.node()) {}

    reference operator*() const { 
        return current_->value; 
    }

    pointer operator->() const { 
        return &current_->value; 
    }

    forward_list_const_iterator& operator++() {
        if (current_) {
            current_ = current_->next;
        }
        return *this;
    }

    forward_list_const_iterator operator++(int) {
        forward_list_const_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const forward_list_const_iterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const forward_list_const_iterator& other) const {
        return current_ != other.current_;
    }

    const typename forward_list<T>::Node* node() const {
        return current_;
    }
};