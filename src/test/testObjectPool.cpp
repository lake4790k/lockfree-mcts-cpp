#include <cstdio>
#include <cassert>
#include "../ObjectPool.hpp"
#include "../Threads.hpp"

class Object {
public:
    Object() {}

    uint64_t data;
};

void test_SingleThreaded_CreateDelete() {
    size_t capacity = 1000;
    ObjectPool<Object> p(capacity);

    Object* numbers[capacity];

    for (size_t i=0; i<capacity; i++) {
        numbers[i] = p.create();
    }
    assert(p.getSize() == capacity);


    for (size_t i=0; i<capacity; i++) {
        p.destroy(numbers[i]);
    }
    assert(p.getSize() == 0);
}

void test_SingleThreaded_Replace() {
    size_t capacity = 10;
    ObjectPool<Object> p(capacity);

    Object* numbers[capacity];

    for (size_t i=0; i<capacity; i++) {
        numbers[i] = p.create();
        numbers[i]->data = i;
    }

    p.destroy(numbers[5]);
    assert(p.getSize() == capacity-1);
    p.destroy(numbers[9]);
    assert(p.getSize() == capacity-2);

    Object* number10 = p.create();
    assert(p.getSize() == capacity-1);
    number10->data = 10;

    Object* number11 = p.create();
    assert(p.getSize() == capacity);
    number11->data = 11;

    Object* memory = p.getMemory();

    for (int i=0; i< capacity; i++) {
        Object* o = memory++;
        if (i==5) {
            assert(o->data == 11);
        } else if (i==9) {
            assert(o->data == 10);
        } else {
            assert(o->data == i);
        }
    }
}

const bool log = false;

void insertDeleteInsert(ObjectPool<Object>& p, size_t times, uint8_t id) {
    Object* o[times];
    for (size_t i=0; i<times; i++) {
        o[i] = p.create();
        if (log) printf("%d created1 %lu = %p\n", id, i, o[i]);
        o[i]->data = id * i;
    }
    for (size_t i=0; i<times; i++) {
        if (log) printf("%d checking1 %lu = %llu\n", id, i, o[i]->data);
        assert(o[i]->data == id*i);
    }    

    for (size_t i=0; i<times; i++) {
        if (log) printf("%d deleted %lu = %p\n", id, i, o[i]);
        p.destroy(o[i]);
    }

    for (size_t i=0; i<times; i++) {
        o[i] = p.create();
        if (log) printf("%d created2 %lu = %p\n", id, i, o[i]);
        o[i]->data = id * i;
    }
    for (size_t i=0; i<times; i++) {
        if (log) printf("%d checking2 %lu = %llu\n", id, i, o[i]->data);
        assert(o[i]->data == id*i);
    }    
}

void test_MultiTreaded(uint8_t threads, size_t times) {
    size_t capacity = 10000;
    ObjectPool<Object> p(capacity);
    Threads pool(4);

    std::vector<std::future<void>> futures;

    for (size_t t=1; t<=threads; t++) {
        futures.emplace_back(pool.invoke([times, &p, t] {
            insertDeleteInsert(p, times, t);
        }));
    }

    for (auto& future : futures)
        future.get();

}


int main() {
    test_SingleThreaded_CreateDelete();
    test_SingleThreaded_Replace();
    test_MultiTreaded(1,10000);
//    test_MultiTreaded(2, 500);
}

