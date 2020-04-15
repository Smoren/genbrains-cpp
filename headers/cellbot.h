#pragma once

#include <map>
#include "cell.h"
#include "directionvector.h"
#include "program.h"
#include "memorystack.h"


namespace GenBrains {

    class CellBot : public Cell
    {
    public:
        CellBot(double energy, double energyLimit, DirectionVector directionVector, Program program);
        virtual ~CellBot();
        virtual void process(Map& map, GroupManager& groupManager);
        void feedPredation(double size, Map& map, GroupManager& groupManager);
        void feedPhoto(double size, Map& map, GroupManager& groupManager);
        void feedMinerals(double size, Map& map, GroupManager& groupManager);
        void convertMineralsToEnergy(Map& map, GroupManager& groupManager);
        void addEnergy(double size, Map& map, GroupManager& groupManager);
        bool subEnergy(double size);
        void addMinerals(double size);
        void subMinerals(double size);
        double getEnergy() const;
        double getMinerals() const;
        const Program& getProgram() const;
        Program& getProgramUnsafe();
        void die();
        bool isDead() const;
        DirectionVector& getDirection();
        void setDirection(const DirectionVector& dv);
        CellBot* clone(const DirectionVector& dv, double energyDivider, int type);
        void divide(Map& map, GroupManager& groupManager, int type, int rotation);
        bool compareProgram(const CellBot* bot, int maxDiff) const;
        const std::map<std::string, double>& getFeedStat() const;
        void updateFeedStat();
        void addViralStat(int underwent, int initiated);
        const std::map<std::string, int>& getViralStat() const;
        double getPredationPower() const;
        MemoryStack& getMemory();
    protected:
        std::map<std::string, double> feedStat;
        int stepsCount;
        std::map<std::string, int> viralStat;
        double energy;
        double energyLimit;
        double minerals;
        Program program;
        DirectionVector direction;
        MemoryStack memory;
    };
}
