#pragma once

#include <map>
#include <set>
#include <stack>
#include <functional>
#include <mutex>
#include "cell.h"
#include "map.h"
#include "../libs/thread-safe-containers-cpp/src/clustergroup.h"


namespace GenBrains {
    class GroupManager
    {
    public:
        GroupManager(Map& map, unsigned long clustersCount);
        ~GroupManager();
        Smoren::ThreadSafeContainers::ClusterGroup<Cell>& getGroup();
        Map& getMap();
        int getSize();
        bool isTerminated();
        void setTerminated();
        void add(Cell* cell);
        void add(Cell *cell, Coords coords);
        void remove(Cell* cell);
        void process(Cell* cell);
        void apply();
        void setProcessHandler(int type, const std::function<void(Cell*, Map&, GroupManager&)> callback);
        void setApplyHandler(int type, const std::function<void(Cell*, Map&, GroupManager&)> callback);
        void setDrawPreset(int type, const std::function<std::vector<double>(Cell*)> callback);
        const std::function<std::vector<double>(Cell*)>& getDrawPreset(int type) const;
        Distributor& getDistributor();
        void printIdUsage();
    protected:
        bool terminated;
        Map& map;
        Smoren::ThreadSafeContainers::ClusterGroup<Cell> group;
        std::map<int, const std::function<void(Cell*, Map&, GroupManager&)>> processHandlers;
        std::map<int, const std::function<void(Cell*, Map&, GroupManager&)>> applyHandlers;
        std::map<int, const std::function<std::vector<double>(Cell*)>> drawPresets;

        int getNextId();
    };
}
