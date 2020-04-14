#include <ctime>
#include <random>
#include "headers/randomizer.h"

namespace GenBrains {
    bool Randomizer::inited = false;

    int Randomizer::getInteger(int start, int range) {
        init();
        if(!range) return 0;
        return rand() % range + start;
    }

    void Randomizer::init() {
        if(!inited) {
            std::srand(unsigned(std::time(0)));
            inited = true;
        }
    }
}

