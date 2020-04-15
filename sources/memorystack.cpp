#include "headers/memorystack.h"

MemoryStack::MemoryStack(int base, int stackSize) : base(base), stackSize(stackSize) {

}

void MemoryStack::push(int what, bool mute) {
    if(mute) mtx.lock();
    if(memory.size() == static_cast<unsigned long>(stackSize)) {
        memory.pop_front();
    }
    memory.push_back(normalize(what));
    if(mute) mtx.unlock();
}

int MemoryStack::pop(bool mute) {
    if(mute) mtx.lock();
    int result;
    if(memory.size()) {
        result = memory.back();
        memory.pop_back();
    } else {
        result = normalize(-1);
    }
    if(mute) mtx.unlock();

    return result;
}

int MemoryStack::add() {
    mtx.lock();
    int result = normalize(pop(false)+pop(false));
    push(result, false);
    mtx.unlock();

    return result;
}

int MemoryStack::add(int with) {
    mtx.lock();
    int result = normalize(pop(false)+with);
    push(result, false);
    mtx.unlock();

    return result;
}

int MemoryStack::sub() {
    mtx.lock();
    int result = normalize(pop(false)-pop(false));
    push(result, false);
    mtx.unlock();

    return result;
}

int MemoryStack::sub(int with) {
    mtx.lock();
    int result = normalize(pop(false)-with);
    push(result, false);
    mtx.unlock();

    return result;
}

bool MemoryStack::equals() {
    mtx.lock();
    int result = pop(false) == pop(false);
    mtx.unlock();

    return result;
}

bool MemoryStack::equals(int with) {
    mtx.lock();
    int result = pop(false) == with;
    mtx.unlock();

    return result;
}

int MemoryStack::normalize(int value) {
    if(value < 0) return base + value;
    if(value >= base) return value - base;
    return value;
}
