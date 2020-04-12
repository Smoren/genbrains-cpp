#pragma once

namespace GenBrains {
    struct Config
    {
    public:
        static const int THREADS = 3;
        static const int CELLS_QUANTUM = 10000;
        static const int STEP_TIMEOUT = 1;
        static const int CELL_SIZE = 4;
        static const int MAP_WIDTH = 400;
        static const int MAP_HEIGHT = 200;
        static const bool MAP_CLOSED_X = true;
        static const bool MAP_CLOSED_Y = false;
        static const bool PRODUCE_ORGANIC = true;

        static bool produceOrganic();
    };
}

