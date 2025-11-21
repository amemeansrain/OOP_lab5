#pragma once
#include <cstddef>
#include <memory_resource>
#include <vector>
#include <new>
#include <stdexcept>

// Кастомный аллокатор памяти для forward_list
class forward_list_memory_resource : public std::pmr::memory_resource {
private:
    // Структура свободного блока
    struct FreeBlock {
        FreeBlock* next_block;
    };

    // Вектор выделенных кусков памяти
    std::vector<void*> memory_chunks_;
    // Список свободных блоков
    FreeBlock* free_blocks_head_;
    // Размер одного блока
    std::size_t block_size_;
    // Количество блоков в одном куске
    std::size_t blocks_per_chunk_;

    // Выделение нового куска памяти
    void allocate_chunk() {
        // Вычисляем общий размер куска
        const std::size_t total_size = block_size_ * blocks_per_chunk_;
        
        // Выделяем сырую память
        void* new_chunk = ::operator new(total_size);
        if (!new_chunk) {
            throw std::bad_alloc();
        }
        
        // Сохраняем указатель для последующего освобождения
        memory_chunks_.push_back(new_chunk);
        
        // Разбиваем кусок на блоки и добавляем в свободный список
        char* byte_ptr = static_cast<char*>(new_chunk);
        for (std::size_t i = 0; i < blocks_per_chunk_; ++i) {
            FreeBlock* block = reinterpret_cast<FreeBlock*>(byte_ptr + i * block_size_);
            block->next_block = free_blocks_head_;
            free_blocks_head_ = block;
        }
    }

public:
    // Конструктор с параметрами
    explicit forward_list_memory_resource(std::size_t block_size, std::size_t chunk_size = 1024)
        : free_blocks_head_(nullptr)
        , block_size_(block_size)
        , blocks_per_chunk_(chunk_size) 
    {
        if (block_size < sizeof(FreeBlock)) {
            block_size_ = sizeof(FreeBlock);
        }
        memory_chunks_.reserve(10); // Резервируем место для 10 кусков
    }

    // Запрещаем копирование и присваивание
    forward_list_memory_resource(const forward_list_memory_resource&) = delete;
    forward_list_memory_resource& operator=(const forward_list_memory_resource&) = delete;

    // Деструктор - освобождаем всю память
    ~forward_list_memory_resource() {
        for (void* chunk : memory_chunks_) {
            ::operator delete(chunk);
        }
    }

protected:
    // Основная функция выделения памяти
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        // Проверяем соответствие размера и выравнивания
        if (bytes != block_size_ || alignment > alignof(std::max_align_t)) {
            throw std::bad_alloc();
        }
        
        // Если нет свободных блоков, выделяем новый кусок
        if (!free_blocks_head_) {
            allocate_chunk();
        }
        
        // Берем первый блок из свободного списка
        FreeBlock* allocated_block = free_blocks_head_;
        free_blocks_head_ = free_blocks_head_->next_block;
        
        return allocated_block;
    }

    // Освобождение памяти (возврат в пул)
    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override {
        if (!ptr || bytes != block_size_ || alignment > alignof(std::max_align_t)) {
            return;
        }
        
        // Возвращаем блок в свободный список
        FreeBlock* block_to_free = static_cast<FreeBlock*>(ptr);
        block_to_free->next_block = free_blocks_head_;
        free_blocks_head_ = block_to_free;
    }

    // Сравнение аллокаторов
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};