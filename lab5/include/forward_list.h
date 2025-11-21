#pragma once
#include <memory_resource>
#include <utility>
#include <stdexcept>
#include "forward_list_iterator.h"

// Предварительное объявление класса аллокатора
class forward_list_memory_resource;

// Шаблонный класс односвязного списка
template <typename T>
class forward_list {
public:
    // Структура узла списка
    struct Node {
        T value;
        Node* next;

        // Конструктор узла с perfect forwarding
        template<typename U>
        Node(U&& val, Node* nxt = nullptr) 
            : value(std::forward<U>(val)), next(nxt) 
        {
        }
    };

private:
    // Тип аллокатора
    using node_allocator = std::pmr::polymorphic_allocator<Node>;
    
    node_allocator node_allocator_;
    Node* list_head_;
    std::size_t element_count_;

    // Создание ресурса памяти по умолчанию
    static forward_list_memory_resource* get_default_memory_resource(std::size_t blocks_per_chunk) {
        static forward_list_memory_resource default_resource(sizeof(Node), blocks_per_chunk);
        return &default_resource;
    }

public:
    // Типы итераторов
    using iterator = forward_list_iterator<T>;
    using const_iterator = forward_list_const_iterator<T>;

    // Размер узла
    static constexpr std::size_t node_size() {
        return sizeof(Node);
    }

    // Конструктор с ресурсом памяти
    explicit forward_list(std::pmr::memory_resource* memory_res = std::pmr::get_default_resource())
        : node_allocator_(memory_res)
        , list_head_(nullptr)
        , element_count_(0) 
    {
    }

    // Конструктор с указанием количества блоков
    explicit forward_list(std::size_t blocks_per_chunk)
        : node_allocator_(get_default_memory_resource(blocks_per_chunk))
        , list_head_(nullptr)
        , element_count_(0) 
    {
    }

    // Деструктор
    ~forward_list() {
        clear_elements();
    }

    // Добавление в начало списка
    template<typename U>
    void push_front(U&& value) {
        // Выделяем память под новый узел
        Node* new_node = node_allocator_.allocate(1);
        
        try {
            // Создаем узел с передачей значения
            node_allocator_.construct(new_node, std::forward<U>(value), list_head_);
        } catch (...) {
            // В случае исключения освобождаем память
            node_allocator_.deallocate(new_node, 1);
            throw;
        }
        
        // Обновляем голову списка и счетчик
        list_head_ = new_node;
        ++element_count_;
    }

    // Удаление из начала списка
    void pop_front() {
        if (!list_head_) {
            return;
        }
        
        Node* node_to_remove = list_head_;
        list_head_ = list_head_->next;
        
        // Уничтожаем объект и освобождаем память
        node_allocator_.destroy(node_to_remove);
        node_allocator_.deallocate(node_to_remove, 1);
        --element_count_;
    }

    // Очистка списка
    void clear_elements() {
        while (list_head_) {
            pop_front();
        }
    }

    // Проверка на пустоту
    bool is_empty() const {
        return list_head_ == nullptr;
    }

    // Количество элементов
    std::size_t get_size() const {
        return element_count_;
    }

    // Доступ к первому элементу
    T& get_front() {
        if (!list_head_) {
            throw std::runtime_error("Empty list - no front element");
        }
        return list_head_->value;
    }

    const T& get_front() const {
        if (!list_head_) {
            throw std::runtime_error("Empty list - no front element");
        }
        return list_head_->value;
    }

    // Итераторы
    iterator begin() {
        return iterator(list_head_);
    }

    iterator end() {
        return iterator(nullptr);
    }

    const_iterator begin() const {
        return const_iterator(list_head_);
    }

    const_iterator end() const {
        return const_iterator(nullptr);
    }

    const_iterator cbegin() const {
        return const_iterator(list_head_);
    }

    const_iterator cend() const {
        return const_iterator(nullptr);
    }

    // Запрещаем копирование и присваивание
    forward_list(const forward_list&) = delete;
    forward_list& operator=(const forward_list&) = delete;

    // Дружественные классы для доступа к узлам
    friend class forward_list_iterator<T>;
    friend class forward_list_const_iterator<T>;
};