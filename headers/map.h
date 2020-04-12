#pragma once

#include <vector>
#include <functional>
#include "cell.h"
#include "distributor.h"


namespace GenBrains {
    class Map
    {
    public:
        static const Coords defaultOffset;

        Map(const int width, const int height, const bool xClosed, const bool yClosed, Distributor distributor);
        ~Map();
        std::vector< std::vector<Cell*> > getData() const;
        int getWidth();
        int getHeight();
        bool isset(const Coords &coords, const Coords &offset = defaultOffset) const;
        Cell* get(const Coords &coords, const Coords &offset = defaultOffset) const;
        void set(Cell* cell, const Coords &coords, const Coords &offset = defaultOffset, bool rewrite = false);
        void remove(const Coords& coords, const Coords& offset, bool setRemoved);
        void remove(Cell* cell, bool setRemoved);
        void move(Cell* cell, const Coords& offset, const std::function<bool(Cell*, Cell*)>& callback);
        Coords formatCoords(const Coords &coords, const Coords &offset = defaultOffset, bool strict = false) const;
        void each(const std::function<void(Cell*)>& callback);
        std::vector<Cell*> findCellsAround(const Coords& coords);
        std::vector<Coords> findEmptyCoordsAround(const Coords& coords);
        Distributor& getDistributor();
        double getDistributionValue(const std::string name, const Coords& coords) const;
    protected:
        static Cell emptyCell;

        int width;
        int height;
        bool xClosed;
        bool yClosed;
        std::vector< std::vector<Cell*> > data;
        std::vector< std::vector<std::mutex*> > lockers;
        Distributor distributor;
    };

    std::ostream& operator <<(std::ostream& stream, const Map& map);
}
