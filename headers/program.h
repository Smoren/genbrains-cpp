#pragma once

#include <vector>
#include <map>
#include <functional>
#include "map.h"
#include "groupmanager.h"
#include "directionvector.h"


namespace GenBrains {
    class CellBot;

    class Program
    {
    public:
        Program(int base, std::vector<int> data, const std::map<int, std::function<void(Program&, CellBot*, Map&, GroupManager&)>>& commands);
        Program(int base, int commandValue, const std::map<int, std::function<void(Program&, CellBot*, Map&, GroupManager&)>>& commands);
        void run(CellBot* bot, Map& map, GroupManager& gm);
        int getBase() const;
        const std::vector<int>& getData() const;
        int getPointer() const;
        void setPointer(int value);
        int getPointerValue() const;
        int getPointerValue(int _pointer) const;
        void setPointerValue(int offset, int value);
        int movePointer(int offset);
        int getMovedPointer(int offset) const;
        std::vector<int> getPart(int start, int count) const;
        void setPart(std::vector<int> part, int start);
        bool canDivide();
        void forbidDivide();
        Program clone() const;
        void stop();
        void actionGrab(CellBot* bot, Map& map, GroupManager& gm);
        void actionMoveDirect(CellBot* bot, Map& map, GroupManager& gm);
        void actionObserve(CellBot* bot, Map& map, const DirectionVector& direction);

    protected:
        int base;
        int pointer;
        bool toStop;
        bool _canDivide;
        std::vector<int> data;
        const std::map<int, std::function<void(Program&, CellBot*, Map&, GroupManager&)>>& commands;
    };
}
