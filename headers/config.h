#pragma once

namespace GenBrains {
    struct Config
    {
    public:
        static const int THREADS = 4;
        static const int CELLS_QUANTUM = 1000;
        static const int STEP_TIMEOUT = 1;
        static const int CELL_SIZE = 2;
        static const int MAP_WIDTH = 900;
        static const int MAP_HEIGHT = 400;
        static const bool MAP_CLOSED_X = true;
        static const bool MAP_CLOSED_Y = false;
        static const bool PRODUCE_ORGANIC = true;
        static const int PROGRAM_BASE = 80;
        static const int MEMORY_STACK_SIZE = 16;

        static bool produceOrganic();
    };
}

