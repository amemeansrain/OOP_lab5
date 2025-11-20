#pragma once
#include <memory_resource>
#include <utility>
#include "forward_list_iterator.h"

class forward_list_memory_resource;

template <typename T>
class forward_list {
public:
    struct Node {
        T value;
        Node* next;

        template<typename U>
        Node(U&& val, Node* nxt = nullptr) 
            : value(std::forward<U>(val)), next(nxt) {}
    };

private:
    using allocator_type = std::pmr::polymorphic_allocator<Node>;
    allocator_type alloc_;
    Node* head_;
    size_t size_;

public:
    using iterator = forward_list_iterator<T>;
    using const_iterator = forward_list_const_iterator<T>;

    static constexpr size_t node_size() {
        return sizeof(Node);
    }

    forward_list(std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : alloc_(mr), head_(nullptr), size_(0) {}

    forward_list(std::size_t blocks_per_chunk)
        : alloc_(create_default_resource(blocks_per_chunk)), head_(nullptr), size_(0) {}

    ~forward_list() {
        clear();
    }

    template<typename U>
    void push_front(U&& value) {
        Node* new_node = alloc_.allocate(1);
        try {
            alloc_.construct(new_node, std::forward<U>(value), head_);
        } catch (...) {
            alloc_.deallocate(new_node, 1);
            throw;
        }
        head_ = new_node;
        size_++;
    }

    void pop_front() {
        if (head_) {
            Node* to_delete = head_;
            head_ = head_->next;
            alloc_.destroy(to_delete);
            alloc_.deallocate(to_delete, 1);
            size_--;
        }
    }

    void clear() {
        while (head_) {
            pop_front();
        }
    }

    bool empty() const {
        return head_ == nullptr;
    }

    size_t size() const {
        return size_;
    }

    T& front() {
        return head_->value;
    }

    const T& front() const {
        return head_->value;
    }

    iterator begin() {
        return iterator(head_);
    }

    iterator end() {
        return iterator(nullptr);
    }

    const_iterator begin() const {
        return const_iterator(head_);
    }

    const_iterator end() const {
        return const_iterator(nullptr);
    }

    const_iterator cbegin() const {
        return const_iterator(head_);
    }

    const_iterator cend() const {
        return const_iterator(nullptr);
    }

private:
    static forward_list_memory_resource* create_default_resource(std::size_t blocks_per_chunk) {
        static forward_list_memory_resource resource(sizeof(Node), blocks_per_chunk);
        return &resource;
    }

    forward_list(const forward_list&) = delete;
    forward_list& operator=(const forward_list&) = delete;

    friend class forward_list_iterator<T>;
    friend class forward_list_const_iterator<T>;
};