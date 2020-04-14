#pragma once

#include <map>
#include <set>
#include <stack>
#include <functional>
#include <mutex>
#include "cell.h"
#include "map.h"
#include "clustermap.h"


namespace GenBrains {
    class GroupManager
    {
    public:
        GroupManager(Map& map);
        ~GroupManager();
        const ClusterMap<Cell*>& getGroup() const;
        Map& getMap();
        int getSize();
        bool isTerminated();
        void setTerminated();
        bool isset(int id);
        void checkExist(int id);
        Cell* get(int id);
        int add(Cell* cell);
        int add(Cell* cell, Coords coords);
        void remove(int id);
        void remove(Cell* cell);
        void removeAll();
        void toAdd(Cell* cell);
        void toAdd(Cell *cell, Coords coords);
        void toRemove(Cell* cell);
        void process(Cell* cell);
        void apply(Cell* cell);
        void apply();
        void applyAdd();
        void applyRemove();
        void setProcessHandler(int type, const std::function<void(Cell*, Map&, GroupManager&)> callback);
        void setApplyHandler(int type, const std::function<void(Cell*, Map&, GroupManager&)> callback);
        void setDrawPreset(int type, const std::function<std::vector<double>(Cell*)> callback);
        const std::function<std::vector<double>(Cell*)>& getDrawPreset(int type) const;
        Distributor& getDistributor();
        ClusterMap<Cell*>::iterator begin();
        ClusterMap<Cell*>::iterator end();
        void each(const std::function<void(Cell*)>& callback);
        void generate();
        bool cannotYield();
        Cell* yield();
        std::mutex& getReadableMutex();
        std::mutex& getWritableMutex();
    protected:
        int lastId;
        bool terminated;
        Map& map;
        ClusterMap<Cell*> group;
        std::stack<Cell*> forAdd;
        std::stack<Cell*> forRemove;
        std::mutex forAddMutex;
        std::mutex forRemoveMutex;
        std::mutex readableMutex;
        std::mutex writableMutex;
        ClusterMap<Cell*>::iterator iter;
        std::map<int, const std::function<void(Cell*, Map&, GroupManager&)>> processHandlers;
        std::map<int, const std::function<void(Cell*, Map&, GroupManager&)>> applyHandlers;
        std::map<int, const std::function<std::vector<double>(Cell*)>> drawPresets;
    };
}
