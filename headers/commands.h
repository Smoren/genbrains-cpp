#pragma once

#include <map>
#include <functional>

namespace GenBrains {
    class Program;
    class CellBot;
    class Map;
    class GroupManager;

    struct Commands
    {
    public:
        static const std::map<int, std::function<void(Program&, CellBot*, Map&, GroupManager&)>> MAP;
    };
}

