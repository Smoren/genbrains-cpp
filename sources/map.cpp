#include <vector>
#include <stdexcept>
#include <functional>
#include "headers/map.h"
#include "headers/cellbot.h"
#include "headers/type.h"
#include "headers/printer.h"


namespace GenBrains {
    const Coords Map::defaultOffset = Coords(0, 0);
    Cell Map::emptyCell = Cell(0);

    Map::Map(
        const int width, const int height, const bool xClosed, const bool yClosed, Distributor distributor
    ) : width(width), height(height), xClosed(xClosed), yClosed(yClosed), distributor(distributor) {
        data.resize(static_cast<unsigned long>(height));
        for(auto& row : data) {
            row.resize(static_cast<unsigned long>(width));
        }
        lockers.resize(static_cast<unsigned long>(height));
        for(auto& row : lockers) {
            for(int i=0; i<width; i++) {
                row.push_back(new std::mutex());
            }
        }
    }

    Map::~Map() {
        for(auto& row : lockers) {
            unsigned long w = static_cast<unsigned long>(width);
            for(unsigned long i=0; i<w; i++) {
                delete row.at(i);
            }
        }
    }

    vector< vector<Cell*> > Map::getData() const {
        return data;
    }

    int Map::getWidth() {
        return width;
    }

    int Map::getHeight() {
        return height;
    }

    bool Map::isset(const Coords& coords, const Coords& offset) const {
        Coords pos = formatCoords(coords, offset);

        if(pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height) {
            stringstream ss;
            ss << "coords out of range (" << pos.x << ", " << pos.y << ")";
            throw std::runtime_error(ss.str());
        }

        return data[static_cast<unsigned long>(pos.y)][static_cast<unsigned long>(pos.x)] != nullptr;
    }

    Cell* Map::get(const Coords& coords, const Coords& offset) const {
        Coords pos = formatCoords(coords, offset);
        Cell* result = data[static_cast<unsigned long>(pos.y)][static_cast<unsigned long>(pos.x)];
        if(result == nullptr) {
            Map::emptyCell = Cell(0);
            return &Map::emptyCell;
        }
        return result;
    }

    void Map::set(Cell* cell, const Coords& coords, const Coords& offset, bool rewrite) {
        Coords pos = formatCoords(coords, offset);
        if(isset(pos, Map::defaultOffset)) {
            if(!rewrite) {
                throw std::runtime_error("cannot rewrite existing cell");
            } else {
                remove(pos, Map::defaultOffset, true);
            }
        }

        cell->setCoords(coords);
        data[static_cast<unsigned long>(coords.y)][static_cast<unsigned long>(coords.x)] = cell;
    }

    void Map::remove(const Coords& coords, const Coords& offset, bool setRemoved) {
        Coords pos = formatCoords(coords, offset);

        //auto* mtx = lockers.at(static_cast<unsigned long>(pos.y)).at(static_cast<unsigned long>(pos.x));
        //mtx->lock();

        if(!isset(pos, Map::defaultOffset)) {
            throw std::runtime_error("cannot remove empty cell");
        }

        get(pos, Map::defaultOffset)->log("removed 1");

        data[static_cast<unsigned long>(pos.y)][static_cast<unsigned long>(pos.x)] = nullptr;

        if(setRemoved) {
            get(coords, offset)->setRemoved();
        }

        //mtx->unlock();
    }

    void Map::remove(Cell* cell, bool setRemoved) {
        //const auto& coords = cell->getCoords();
        //auto* mtx = lockers.at(static_cast<unsigned long>(coords.y)).at(static_cast<unsigned long>(coords.x));
        //mtx->lock();

        Coords pos = cell->getCoords();
        if(!isset(pos, Map::defaultOffset)) {
            throw std::runtime_error("cannot remove empty cell");
        }

        data[static_cast<unsigned long>(pos.y)][static_cast<unsigned long>(pos.x)] = nullptr;

        cell->log("removed 2");

        if(setRemoved) {
            cell->setRemoved();
        }

        //mtx->unlock();
    }

    void Map::move(Cell* cell, const Coords& offset, const std::function<bool(Cell*, Cell*)>& callback) {
        if(offset.isNullVector()) {
            return;
        }

        const Coords& coords = cell->getCoords();
        Coords newCoords = formatCoords(coords, offset);

        if(coords == newCoords) {
            return;
        }

        auto* lhsMutex = lockers.at(static_cast<unsigned long>(coords.y)).at(static_cast<unsigned long>(coords.x));
        auto* rhsMutex = lockers.at(static_cast<unsigned long>(newCoords.y)).at(static_cast<unsigned long>(newCoords.x));

        if(lhsMutex == rhsMutex) {
            throw std::runtime_error("mutex equals");
        }

        // TODO unique_lock?

        //lhsMutex->lock();
        //rhsMutex->lock();

        if(newCoords == coords) {
            return;
        }

        if(cell->isEmpty()) {
            throw std::runtime_error("cannot move empty cell");
        }

        Cell* anotherCell = get(newCoords, Map::defaultOffset);

        if(callback(cell, anotherCell) && !cell->isRemoved()) {
            stringstream ss;
            ss << "moved" << coords << " -> " << newCoords;
            cell->log(ss.str());

            try {
                remove(cell, false);
            } catch(std::runtime_error e) {
                // TODO костыль...
                cell->log("bad thing");
                std::cout << "bad thing with type " << cell->getType() << std::endl;
            }
            set(cell, newCoords, Map::defaultOffset, true); // а мы не уверены, что не перезапишем существующую((
        }

        //lhsMutex->unlock();
        //rhsMutex->unlock();
    }

    Coords Map::formatCoords(const Coords& coords, const Coords& offset, bool strict) const {
        if(offset.x == 0 && offset.y == 0 && coords.x >= 0 && coords.x < width && coords.y >= 0 && coords.y < height) {
            return coords.clone();
        }

        Coords result(coords.x+offset.x, coords.y+offset.y);
        if(result.x < 0) {
            if(xClosed) result.x = width + result.x;
            else if(!strict) result.x = coords.x;
            else throw std::runtime_error("less than x bound");
        } else if(result.x >= width) {
            if(xClosed) result.x = result.x % width;
            else if(!strict) result.x = coords.x;
            else throw std::runtime_error("more than x bound");
        }
        if(result.y < 0) {
            if(yClosed) result.y = height + result.y;
            else if(!strict) result.y = coords.y;
            else throw std::runtime_error("less than y bound");
        } else if(result.y >= height) {
            if(yClosed) result.y = result.y % height;
            else if(!strict) result.y = coords.y;
            else throw std::runtime_error("more than y bound");
        }

        if(result.x < 0) result.x = 0;
        else if(result.x >= width) result.x = width;

        if(result.y < 0) result.y = 0;
        else if(result.y >= height) result.y = width;

        return result;
    }

    void Map::each(const std::function<void(Cell*)>& callback) {
        for(int y=0; y<data.size(); y++) {
            for(int x=0; x<data[y].size(); x++) {
                callback(get(Coords(x, y)));
            }
        }
    }

    std::vector<Cell*> Map::findCellsAround(const Coords& coords) {
        std::vector<Cell*> result;
        for(int dx=-1; dx<=1; dx++) {
            for(int dy=-1; dy<=1; dy++) {
                if(dx == 0 && dx == dy) continue;
                try {
                    Coords crds = formatCoords(coords, DirectionVector(dx, dy), true);
                    if(isset(crds)) {
                        result.push_back(get(crds));
                    }
                } catch(std::runtime_error e) {}
            }
        }
        return result;
    }

    Distributor& Map::getDistributor() {
        return distributor;
    }

    std::vector<Coords> Map::findEmptyCoordsAround(const Coords& coords) {
        std::vector<Coords> result;
        for(int dx=-1; dx<=1; dx++) {
            for(int dy=-1; dy<=1; dy++) {
                if(dx == 0 && dx == dy) continue;
                try {
                    Coords crds = formatCoords(coords, DirectionVector(dx, dy), true);
                    if(!isset(crds)) {
                        result.push_back(crds);
                    }
                } catch(std::runtime_error e) {}
            }
        }
        return result;
    }

    double Map::getDistributionValue(const std::string name, const Coords& coords) const {
        return distributor.getDistributionValue(name, coords, width, height);
    }

    ostream& operator <<(std::ostream& stream, const Map& map) {
        stringstream result;

        auto data = map.getData();
        for(unsigned long y=0; y<data.size(); y++) {
            for(unsigned long x=0; x<data[y].size(); x++) {
                result << *map.get(Coords(static_cast<int>(x), static_cast<int>(y))) << '\t';
            }

            result << '\n';
        }

        return stream << result.str();
    }
}
