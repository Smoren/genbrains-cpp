#include <stdexcept>
#include <sstream>
#include "headers/groupmanager.h"
#include "headers/commands.h"

namespace GenBrains {
    GroupManager::GroupManager(
        Map& map
    ) : lastId(0), terminated(false), map(map),
        group(ClusterMap<Cell*>(static_cast<unsigned long>(map.getWidth()*1), static_cast<unsigned long>(map.getHeight()))),
        idLimit(map.getWidth()*map.getHeight()*1),
        iter(group.begin()) {

    }

    GroupManager::~GroupManager() {
        // TODO проверить
        std::cout << "destruct start" << std::endl;
        removeAll();
        std::cout << "destruct end" << std::endl;
    }

    bool GroupManager::isTerminated() {
        return terminated;
    }

    void GroupManager::setTerminated() {
        terminated = true;
    }

    ClusterMap<Cell*>& GroupManager::getGroup() {
        return group;
    }

    Map& GroupManager::getMap() {
        return map;
    }

    int GroupManager::getSize() {
        return static_cast<int>(group.size());
    }

    bool GroupManager::isset(int id) {
        if(group.find(static_cast<unsigned long>(id)) == group.end()) {
            return false;
        }

        return true;
    }

    void GroupManager::checkExist(int id) {
        if(!isset(id)) {
            std::stringstream ss;
            ss << "cell is not exist (id: " << id << ")";
            throw std::runtime_error(ss.str());
        }
    }

    Cell* GroupManager::get(int id) {
        checkExist(id);

        return group.at(static_cast<unsigned long>(id));
    }

    int GroupManager::add(Cell* cell) {
        forAddMutex.lock();
        int id = getNextId();
        cell->setId(id);
        forAddMutex.unlock();
        group.insert({id, cell});

        return lastId;
    }

    int GroupManager::add(Cell* cell, Coords coords) {
        add(cell);
        map.set(cell, coords, Map::defaultOffset, true);
        return lastId;
    }

    void GroupManager::remove(int id) {
        Cell* cell = get(id);

        try {
            map.remove(cell, true);
        } catch(std::runtime_error e) {}

        group.erase(static_cast<unsigned long>(id));
        delete cell;
    }

    void GroupManager::remove(Cell* cell) {
        try {
            map.remove(cell, true);
        } catch(std::runtime_error e) {}

        group.erase(static_cast<unsigned long>(cell->getId()));
        delete cell;
    }

    void GroupManager::removeAll() {
        Cell* cell;
        generate();
        while((cell = yield()) != nullptr) {
            remove(cell);
        }
        std::cout << "all removed" << std::endl;
    }

    void GroupManager::toAdd(Cell *cell) {
        map.set(cell, cell->getCoords(), Map::defaultOffset, true);
        forAddMutex.lock();
        forAdd.push(cell);
        forAddMutex.unlock();
    }

    void GroupManager::toAdd(Cell *cell, Coords coords) {
        map.set(cell, coords, Map::defaultOffset, true);
        forAddMutex.lock();
        forAdd.push(cell);
        forAddMutex.unlock();
    }

    void GroupManager::toRemove(Cell *cell) {
        forRemoveMutex.lock();
        forRemove.push(cell);
        forRemoveMutex.unlock();
    }

    void GroupManager::process(Cell* cell) {
        if(cell->isRemoved()) {
            toRemove(cell);
            return;
        }

        int type = cell->getType();
        if(processHandlers.find(type) == processHandlers.end()) {
            return;
        }
        processHandlers.at(type)(cell, map, *this);

        if(cell->isRemoved()) {
            toRemove(cell);
        }
    }

    void GroupManager::apply(Cell* cell) {
        int type = cell->getType();
        if(applyHandlers.find(type) == applyHandlers.end()) {
            return;
        }
        applyHandlers.at(type)(cell, map, *this);
    }

    void GroupManager::apply() {
        writableMutex.lock();

        applyAdd();
        applyRemove();

        writableMutex.unlock();
    }

    Cell* GroupManager::getCellForApplyAdd() {
        forAddMutex.lock();
        if(!forAdd.size()) {
            forAddMutex.unlock();
            return nullptr;
        }
        auto* result = forAdd.top();
        forAdd.pop();
        forAddMutex.unlock();

        return result;
    }

    Cell* GroupManager::getCellForApplyRemove() {
        forRemoveMutex.lock();
        if(!forRemove.size()) {
            forRemoveMutex.unlock();
            return nullptr;
        }
        auto* result = forRemove.top();
        forRemove.pop();
        forRemoveMutex.unlock();

        return result;
    }

    void GroupManager::applyAdd() {
        forAddMutex.lock();
        while(forAdd.size()) {
            Cell* cell = forAdd.top();
            forAdd.pop();
            add(cell);
        }
        forAddMutex.unlock();
    }

    void GroupManager::applyRemove() {
        forRemoveMutex.lock();
        while(forRemove.size()) {
            Cell* cell = forRemove.top();
            forRemove.pop();
            try {
                remove(cell);
            } catch(std::runtime_error e) {}
        }
        forRemoveMutex.unlock();
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

    ClusterMap<Cell*>::iterator GroupManager::begin() {
        return group.begin();
    }

    ClusterMap<Cell*>::iterator GroupManager::end() {
        return group.end();
    }

    void GroupManager::each(const std::function<void(Cell*)>& callback) {
        for(auto& [id, cell] : group) {
            callback(cell);
        }
    }

    void GroupManager::generate() {
        iter = group.begin();
    }

    bool GroupManager::cannotYield() {
        return iter == group.end();
    }

    Cell* GroupManager::yield() {
        if(iter == group.end()) {
            return nullptr;
        }

        Cell* result = (*iter).second;
        iter++;

        return result;
    }

    std::mutex& GroupManager::getReadableMutex() {
        return readableMutex;
    }

    std::mutex& GroupManager::getWritableMutex() {
        return writableMutex;
    }

    void GroupManager::printIdUsage() {
        cout << "lastId: " << lastId << "/" << idLimit << endl;
    }

    int GroupManager::getNextId() {
        if(++lastId >= idLimit) {
            lastId = 0;
        }

        while(isset(lastId)) {
            ++lastId;
        }

        return lastId;
    }
}
