#pragma once
#include <iterator>

// Предварительное объявление класса списка
template <typename T>
class forward_list;

// Итератор для односвязного списка
template <typename T>
class forward_list_iterator {
private:
    // Текущий узел в итераторе
    typename forward_list<T>::Node* current_node_;

public:
    // Стандартные типы для итератора
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    // Явный конструктор
    explicit forward_list_iterator(typename forward_list<T>::Node* node = nullptr) 
        : current_node_(node) 
    {
    }

    // Оператор разыменования
    reference operator*() const { 
        if (!current_node_) {
            throw std::runtime_error("Dereferencing null iterator");
        }
        return current_node_->value; 
    }

    // Оператор доступа к члену
    pointer operator->() const { 
        if (!current_node_) {
            throw std::runtime_error("Accessing member through null iterator");
        }
        return &current_node_->value; 
    }

    // Префиксный инкремент
    forward_list_iterator& operator++() {
        if (current_node_) {
            current_node_ = current_node_->next;
        }
        return *this;
    }

    // Постфиксный инкремент
    forward_list_iterator operator++(int) {
        forward_list_iterator old_state = *this;
        this->operator++();
        return old_state;
    }

    // Операторы сравнения
    bool operator==(const forward_list_iterator& other) const {
        return current_node_ == other.current_node_;
    }

    bool operator!=(const forward_list_iterator& other) const {
        return !(*this == other);
    }

    // Получение узла (для внутреннего использования)
    typename forward_list<T>::Node* get_node() const {
        return current_node_;
    }
};

// Константный итератор
template <typename T>
class forward_list_const_iterator {
private:
    const typename forward_list<T>::Node* current_node_;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    // Конструкторы
    explicit forward_list_const_iterator(const typename forward_list<T>::Node* node = nullptr) 
        : current_node_(node) 
    {
    }
    
    // Конструктор преобразования из неконстантного итератора
    forward_list_const_iterator(const forward_list_iterator<T>& other)
        : current_node_(other.get_node()) 
    {
    }

    reference operator*() const { 
        if (!current_node_) {
            throw std::runtime_error("Dereferencing null const iterator");
        }
        return current_node_->value; 
    }

    pointer operator->() const { 
        if (!current_node_) {
            throw std::runtime_error("Accessing member through null const iterator");
        }
        return &current_node_->value; 
    }

    forward_list_const_iterator& operator++() {
        if (current_node_) {
            current_node_ = current_node_->next;
        }
        return *this;
    }

    forward_list_const_iterator operator++(int) {
        forward_list_const_iterator old_state = *this;
        ++(*this);
        return old_state;
    }

    bool operator==(const forward_list_const_iterator& other) const {
        return current_node_ == other.current_node_;
    }

    bool operator!=(const forward_list_const_iterator& other) const {
        return !(*this == other);
    }

    const typename forward_list<T>::Node* get_node() const {
        return current_node_;
    }
};