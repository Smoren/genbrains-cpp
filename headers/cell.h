#pragma once

#include <mutex>
#include <deque>
#include "coords.h"

namespace GenBrains {
    class Map;
    class GroupManager;

    class Cell
    {
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
        unsigned long getClusterId() const;
        void setClusterId(unsigned long clusterId);
        void removeClusterId();
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


