#include <algorithm>
#include "config.h"
#include "cellbot.h"
#include "groupmanager.h"
#include "map.h"
#include "type.h"
#include "randomizer.h"


namespace GenBrains {
    CellBot::CellBot(
        double energy, double energyLimit, DirectionVector directionVector, Program program
    ) : Cell(Type::BOT), stepsCount(0), energy(energy), energyLimit(energyLimit), minerals(0), program(program), direction(directionVector) {
        feedStat.insert({"predation", 0});
        feedStat.insert({"photo", 0});
        feedStat.insert({"minerals", 0});
        viralStat.insert({"underwent", 0});
        viralStat.insert({"initiated", 0});
    }

    CellBot::~CellBot() {

    }

    void CellBot::process(Map& map, GroupManager& groupManager) {
        if(removed) return;
        //std::lock_guard<std::mutex> lhsLock(*mutex);
        program.run(this, map, groupManager);
        stepsCount++;
    }

    void CellBot::feedPredation(double size, Map& map, GroupManager& groupManager) {
        addEnergy(size, map, groupManager);
        feedStat["predation"] += size;
    }

    void CellBot::feedPhoto(double size, Map& map, GroupManager& groupManager) {
        addEnergy(size, map, groupManager);
        feedStat["photo"] += size;
    }

    void CellBot::feedMinerals(double size, Map& map, GroupManager& groupManager) {
        minerals += size;
    }

    void CellBot::convertMineralsToEnergy(Map& map, GroupManager& groupManager) {
        int mineralsQuantum = 5; // TODO меньше? 2
        if(minerals > mineralsQuantum) {
            minerals -= mineralsQuantum;
            feedStat["minerals"] += mineralsQuantum;
            addEnergy(mineralsQuantum, map, groupManager);
        }
    }

    void CellBot::addEnergy(double size, Map& map, GroupManager& groupManager) {
        energy += size;
        if(energy > energyLimit) {
            energy = energyLimit;
            divide(map, groupManager, Type::BOT, 4);
        }
    }

    void CellBot::addMinerals(double size) {
        minerals += size;
    }

    void CellBot::subMinerals(double size) {
        minerals -= size;
    }

    bool CellBot::subEnergy(double size) {
        energy -= size;
        if(energy <= 0) {
            die();
            return false;
        }
        return true;
    }

    double CellBot::getEnergy() const {
        return energy;
    }

    double CellBot::getMinerals() const {
        return minerals;
    }

    const Program& CellBot::getProgram() const {
        return program;
    }

    Program& CellBot::getProgramUnsafe() {
        return program;
    }

    void CellBot::die() {
        if(!Config::produceOrganic()) {
            setRemoved();
        }
        type = Type::ORGANIC;
        energy = 0;
        program.stop();
    }

    bool CellBot::isDead() const {
        return type != Type::BOT;
    }

    DirectionVector& CellBot::getDirection() {
        return direction;
    }

    void CellBot::setDirection(const DirectionVector& dv) {
        direction = dv;
    }

    CellBot* CellBot::clone(const DirectionVector& dv, double energyDivider, int type) {
        energy /= energyDivider;
        CellBot* result = new CellBot(energy, energyLimit, dv, program.clone());
        result->setType(type);

        return result;
    }

    void CellBot::divide(Map& map, GroupManager& groupManager, int type, int rotation) {
        if(!program.canDivide()) {
            return;
        }
        DirectionVector offset = direction.getNewDirection(rotation);
        Coords crds = map.formatCoords(coords, offset);
        if(!map.isset(crds)) {
            groupManager.toAdd(clone(offset, 3, type), crds);
        } else {
            const auto& variants = map.findEmptyCoordsAround(coords);
            const int variantsSize = static_cast<int>(variants.size());
            if(variantsSize == 0) {
                die();
            } else {
                crds = variants[static_cast<unsigned long>(Randomizer::getInteger(0, variantsSize))];
                groupManager.toAdd(clone(offset, 3, type), crds);
            }
        }

        program.stop();
    }

    bool CellBot::compareProgram(const CellBot* bot, int maxDiff) const {
        int diffCounter = 0;
        auto& lhsProgramData = program.getData();
        auto& rhsProgramData = bot->getProgram().getData();
        unsigned long base = static_cast<unsigned long>(bot->getProgram().getBase());
        // TODO разные базы???
        for(unsigned long i=0; i<base; i++) {
            if(lhsProgramData[i] != rhsProgramData[i]) {
                diffCounter++;
                if(diffCounter > maxDiff) {
                    return false;
                }
            }
        }
        return true;
    }

    const std::map<std::string, double>& CellBot::getFeedStat() const {
        return feedStat;
    }

    void CellBot::updateFeedStat() {
        double minVal = std::min({feedStat.at("photo"), feedStat.at("minerals"), feedStat.at("predation")});
        for(auto& item : feedStat) {
            item.second -= minVal;
        }
    }

    void CellBot::addViralStat(int underwent, int initiated) {
        viralStat.at("underwent") += underwent;
        viralStat.at("initiated") += initiated;
    }

    const std::map<std::string, int>& CellBot::getViralStat() const {
        return viralStat;
    }

    double CellBot::getPredationPower() const {
        const double divider = feedStat.at("photo") + feedStat.at("minerals");
        if(divider < 0.0001) return feedStat.at("predation");
        return feedStat.at("predation")/divider;
    }
}

