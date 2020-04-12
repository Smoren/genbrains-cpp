#pragma once

#include <thread>
#include <mutex>
#include "groupmanager.h"


namespace GenBrains {
    class Drawer
    {
    public:
        static void init(GroupManager& gm, Map& map, std::thread& process, int cellSize);
        static void start();
        static void pause();
        static void timer(int value);
        static void render();
        static void destroy();
    protected:
        static GroupManager* gm;
        static Map* map;
        static std::thread* process;
        static int cellSize;
        static bool isActive;
        static std::map<int, const std::function<void(Cell&)>&> drawHandlers;

        static void clear();
        static void drawCell(Coords coords, std::vector<double> color);
    };
}
