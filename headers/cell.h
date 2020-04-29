#pragma once

#include <mutex>
#include <deque>
#include "coords.h"
#include "../libs/thread-safe-containers-cpp/src/clustergroup.h"

namespace GenBrains {
    class Map;
    class GroupManager;

    class Cell : public Smoren::ThreadSafeContainers::ClusterItem {
    public:
        Cell(int type);
        virtual ~Cell();
        bool isEmpty() const;
        int getType() const;
        void setType(int _type);
        Coords getCoords() const;
        void setCoords(Coords _coords);
        void removeCoords();
        int getId() const;
        void setId(int _id);
        virtual unsigned long getClusterId() const;
        virtual void setClusterId(unsigned long clusterId);
        virtual void removeClusterId();
        bool isRemoved();
        void setRemoved();
        virtual void process(Map& map, GroupManager& groupManager);
        void removeId();
        std::mutex* getMutex() const;
        void log(std::string message);
    protected:
        int type;
        int id;
        unsigned long clusterId;
        bool removed = false;
        Coords coords;
        mutable std::mutex* mutex;
        std::deque<std::string> logStorage;
    };

    std::ostream& operator <<(std::ostream& stream, const Cell& cell);
}


