#pragma once
#include <cstddef>
#include <memory_resource>
#include <vector>
#include <new>

class forward_list_memory_resource : public std::pmr::memory_resource {
private:
    struct free_block {
        free_block* next;
    };

    std::vector<void*> allocated_chunks_;
    free_block* free_list_ = nullptr;
    std::size_t block_size_;
    std::size_t chunk_size_;

    void allocate_new_chunk() {
        void* chunk = ::operator new(block_size_ * chunk_size_);
        allocated_chunks_.push_back(chunk);
        
        for (std::size_t i = 0; i < chunk_size_; ++i) {
            free_block* block = static_cast<free_block*>(
                static_cast<void*>(static_cast<char*>(chunk) + i * block_size_)
            );
            block->next = free_list_;
            free_list_ = block;
        }
    }

public:
    explicit forward_list_memory_resource(std::size_t block_size, std::size_t chunk_size = 1024)
        : block_size_(block_size), chunk_size_(chunk_size) {
        allocated_chunks_.reserve(10);
    }

    ~forward_list_memory_resource() {
        for (void* chunk : allocated_chunks_) {
            ::operator delete(chunk);
        }
    }

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        if (bytes != block_size_ || alignment > alignof(std::max_align_t)) {
            throw std::bad_alloc();
        }
        
        if (!free_list_) {
            allocate_new_chunk();
        }
        
        free_block* block = free_list_;
        free_list_ = free_list_->next;
        return block;
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        if (bytes != block_size_ || alignment > alignof(std::max_align_t)) {
            return;
        }
        
        free_block* block = static_cast<free_block*>(p);
        block->next = free_list_;
        free_list_ = block;
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};