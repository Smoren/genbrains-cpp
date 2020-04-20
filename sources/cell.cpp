#include "headers/cell.h"

namespace GenBrains {
    Cell::Cell(int type) : type(type), id(-1), clusterId(0), coords(Coords(-1, -1)) {
        mutex = new std::mutex();
    }

    Cell::~Cell() {
        delete mutex;
    }

    bool Cell::isEmpty() const {
        return !type;
    }

    int Cell::getType() const {
        return type;
    }

    void Cell::setType(int _type) {
        type = _type;
    }

    Coords Cell::getCoords() const {
        return coords;
    }

    void Cell::setCoords(Coords _coords) {
        coords.x = _coords.x;
        coords.y = _coords.y;
    }

    void Cell::removeCoords() {
        coords.x = -1;
        coords.y = -1;
    }

    int Cell::getId() const {
        return id;
    }

    void Cell::setId(int _id) {
        id = _id;
    }

    void Cell::removeId() {
        id = -1;
    }

    bool Cell::isRemoved() {
        return removed;
    }

    void Cell::setRemoved() {
        log("set removed");
        //removeCoords();
        removed = true;
    }

    std::mutex* Cell::getMutex() const {
        return mutex;
    }

    void Cell::log(std::string message) {
        //if(logStorage.size() == 10) {
        //    logStorage.pop_front();
        //}
        //logStorage.push_back(message);
    }

    void Cell::process(Map &map, GroupManager& groupManager) {

    }

    unsigned long Cell::getClusterId() const {
        return clusterId;
    }

    void Cell::setClusterId(unsigned long clusterId) {
        this->clusterId = clusterId;
    }

    void Cell::removeClusterId() {
        clusterId = 0;
    }



    std::ostream& operator <<(std::ostream& stream, const Cell& cell) {
        if(cell.getType()) {
            return stream << cell.getId();
        }
        return stream << cell.getType();
    }
}
