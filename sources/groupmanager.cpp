#include <stdexcept>
#include <sstream>
#include "headers/groupmanager.h"
#include "headers/commands.h"

namespace GenBrains {
    GroupManager::GroupManager(
        Map& map, unsigned long clustersCount
    ) : terminated(false), map(map),
        group(ClusterGroup<Cell>(clustersCount)) {

    }

    GroupManager::~GroupManager() {
        // TODO проверить
        std::cout << "destruct start" << std::endl;
        //removeAll();
        std::cout << "destruct end" << std::endl;
    }

    bool GroupManager::isTerminated() {
        return terminated;
    }

    void GroupManager::setTerminated() {
        terminated = true;
    }

    ClusterGroup<Cell>& GroupManager::getGroup() {
        return group;
    }

    Map& GroupManager::getMap() {
        return map;
    }

    int GroupManager::getSize() {
        return static_cast<int>(group.getItemsCount());
    }

//    bool GroupManager::isset(int id) {
//        if(group.find(static_cast<unsigned long>(id)) == group.end()) {
//            return false;
//        }

//        return true;
//    }

//    void GroupManager::checkExist(int id) {
//        if(!isset(id)) {
//            std::stringstream ss;
//            ss << "cell is not exist (id: " << id << ")";
//            throw std::runtime_error(ss.str());
//        }
//    }

//    Cell* GroupManager::get(int id) {
//        checkExist(id);

//        return group.at(static_cast<unsigned long>(id));
//    }

    void GroupManager::add(Cell* cell) {
        group.add(cell);
    }

    void GroupManager::add(Cell* cell, Coords coords) {
        add(cell);
        map.set(cell, coords, Map::defaultOffset, true);
    }


    void GroupManager::remove(Cell* cell) {
        try {
            map.remove(cell, true);
        } catch(std::runtime_error e) {}

        group.remove(cell);
    }

//    void GroupManager::removeAll() {
//        Cell* cell;
//        generate();
//        while((cell = yield()) != nullptr) {
//            remove(cell);
//        }
//        std::cout << "all removed" << std::endl;
//    }

    void GroupManager::process(Cell* cell) {
        if(cell->isRemoved()) {
            remove(cell);
            return;
        }

        int type = cell->getType();
        if(processHandlers.find(type) == processHandlers.end()) {
            return;
        }
        processHandlers.at(type)(cell, map, *this);

        if(cell->isRemoved()) {
            remove(cell);
        }
    }

    void GroupManager::apply() {
        group.apply();
    }

    void GroupManager::setProcessHandler(int type, const std::function<void(Cell*, Map&, GroupManager&)> callback) {
        processHandlers.insert({type, callback});
    }

    void GroupManager::setApplyHandler(int type, const std::function<void(Cell*, Map&, GroupManager&)> callback) {
        applyHandlers.insert({type, callback});
    }

    void GroupManager::setDrawPreset(int type, const std::function<std::vector<double>(Cell*)> callback) {
        drawPresets.insert({type, callback});
    }

    const std::function<std::vector<double>(Cell*)>& GroupManager::getDrawPreset(int type) const {
        if(drawPresets.find(type) == drawPresets.end()) {
            std::stringstream ss;
            ss << "type is not exist (" << type << ")";
            throw std::runtime_error(ss.str());
        }
        return drawPresets.at(type);
    }

    Distributor& GroupManager::getDistributor() {
        return map.getDistributor();
    }
}
