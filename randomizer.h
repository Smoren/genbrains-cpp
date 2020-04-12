#pragma once


namespace GenBrains {
    class Randomizer
    {
    public:
        static int getInteger(int start, int range);
    protected:
        static bool inited;
        static void init();
    };
}
