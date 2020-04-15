#pragma once

#include <deque>
#include <mutex>

class MemoryStack {
public:
    MemoryStack(int base, int stackSize);
    void push(int what, bool mute=true);
    int pop(bool mute=true);
    int add();
    int add(int with);
    int sub();
    int sub(int with);
    bool equals();
    bool equals(int with);
protected:
    int base;
    int stackSize;
    std::deque<int> memory;
    std::mutex mtx;

    int normalize(int value);
};
