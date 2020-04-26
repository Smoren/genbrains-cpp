#include <iostream>
#include <map>
#include <thread>
#include <math.h>
#include <ctime>
#include "headers/printer.h"
#include "headers/groupmanager.h"
#include "headers/cellbot.h"
#include "headers/directionvector.h"
#include "headers/drawer.h"
#include "headers/distributor.h"
#include "headers/threading.h"
#include "headers/config.h"
#include "headers/type.h"
#include "headers/commands.h"
#include "headers/randomizer.h"
#include "headers/memorystack.h"

using namespace GenBrains;


int main() {
    std::srand(unsigned(std::time(0)));

    Distributor distributor(1.0, [](const Distributor& dist, double argument, double state) -> double {
        return std::cos(argument/M_PI)/2+0.5;
    });
    distributor.addDistribution("photo", [](const Distributor& dist, const Coords& coords, int width, int height) -> double {
        return 1/std::exp(coords.y/(height/8))*7*dist.getState();
    });
    distributor.addDistribution("minerals", [](const Distributor& dist, const Coords& coords, int width, int height) -> double {
        const double k = 0.35;
        if(coords.y < height*k) {
            return 0;
        } else {
            return (coords.y-height*k)/(height-height*k)*4;
        }
    });

    Map map(Config::MAP_WIDTH, Config::MAP_HEIGHT, Config::MAP_CLOSED_X, Config::MAP_CLOSED_Y, distributor);

    GroupManager gm(map, Config::THREADS);

    gm.setProcessHandler(Type::BOT, [](Cell* cell, Map& map, GroupManager& gm) {
        cell->process(map, gm);
    });
    gm.setProcessHandler(Type::ORGANIC, [](Cell* cell, Map& map, GroupManager& gm) {
        map.move(cell, Coords(0, 1), [](Cell* lhs, Cell* rhs) {
            return rhs->getType() == Type::EMPTY;
        });
    });
    gm.setDrawPreset(Type::BOT, [](Cell* cell) -> std::vector<double> {
        if(cell->isRemoved()) return {0, 0, 0};

        std::vector<double> result;

        CellBot* bot = dynamic_cast<CellBot*>(cell);
        const auto& feedStat = bot->getFeedStat();

//        const auto& viralStat = bot->getViralStat();
//        if(viralStat.at("initiated")) {
//            return {1.0, 0.0, 0.0};
//        } else if(viralStat.at("underwent")) {
//            return {0.0, 0.0, 1.0};
//        } else {
//            return {1.0, 1.0, 1.0};
//        }

        const double predation = feedStat.at("predation");
        const double photo = feedStat.at("photo");
        const double minerals = feedStat.at("minerals");

        double max = std::max({predation, photo, minerals});
        if(max < 0.0001) {
            result = {1.0, 1.0, 1.0};
        } else {
            result = {predation/max, photo/max, minerals/max};
        }

        const auto& viralStat = bot->getViralStat();
        if(viralStat.at("initiated") || viralStat.at("underwent")) {
            for(auto& item : result) {
                item /= 2;
            }
        }

        return result;
    });
    gm.setDrawPreset(Type::ORGANIC, [](Cell* cell) -> std::vector<double> {
        return {0.25, 0.25, 0.25};
    });
    gm.setDrawPreset(Type::POISON, [](Cell* cell) -> std::vector<double> {
        return {0.5, 0.5, 0.5};
    });
    gm.setDrawPreset(Type::WALL, [](Cell* cell) -> std::vector<double> {
        return {0.8, 0.8, 0.8};
    });

    auto* bot = new CellBot(10, 100, DirectionVector(1, 1), Program(Config::PROGRAM_BASE, 40, Commands::MAP));
    bot->getDirection().randomize();
    gm.add(bot);
    map.set(bot, Coords(100, 60), Map::defaultOffset);

    std::thread process(threadProcess, std::ref(gm));
    Drawer::init(gm, map, process, Config::CELL_SIZE);
    process.join();

    std::cout << "the end" << std::endl;
    return 0;
}
