#pragma once

#include <new>
#include <atomic>
#include <cstdlib>
#include <cstdio>
#include <cassert>

template<typename C>
class ObjectPool {
public:

    ObjectPool(uint64_t capacity) {
        memory = malloc(capacity * sizeof(C));
        next = static_cast<C*>(memory);
        end = next + capacity;
        free1 = new C*[capacity];
        frees = free1;
    }


    ~ObjectPool() {
        free(memory);
        delete[] free1;
    }

    C* create() {
        size++;
        C** free = frees.load();
        if (free > free1) {
            free = frees.fetch_sub(1);
            free--;
            if (free >= free1) {
                if (*free < memory) printf("WTF=%p\n", *free);
                assert(*free>= memory);
                assert(*free <= end);
                return *free;
            }
        }

        C* next1 = next.fetch_add(1);
        assert(next1 <= end);
            
        new (next1) C();
        return next1;
    }

    void destroy(C* o) {
        size--;
        C** free = frees.fetch_add(1);
        while (free < free1) {
            free = frees.fetch_add(1);
        }
        *free = o;
    }

    uint64_t getSize() {
        return size;
    }

    C* getMemory() {
        return static_cast<C*>(memory);
    }

private:
    void* memory;
    std::atomic<C*> next;
    C* end;

    std::atomic<C**> frees;
    C** free1;

    std::atomic<uint64_t> size{0};

};