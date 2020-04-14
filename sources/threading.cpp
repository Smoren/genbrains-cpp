#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <mutex>
#include <unistd.h>
#include "headers/threading.h"
#include "headers/groupmanager.h"
#include "headers/cell.h"
#include "headers/drawer.h"
#include "headers/config.h"

namespace GenBrains {
    void threadProcess(GroupManager& gm) {
        std::cout << "process started step " << std::endl;

        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();

        gm.generate();

        int i=0;
        while(true) {
            std::chrono::time_point<std::chrono::system_clock> start, end;
            start = std::chrono::system_clock::now();

            //std::cout << "========================= step " << i << " started" << std::endl;

            std::vector<std::thread> subprocesses;

            for(int j=0; j<Config::THREADS; j++) {
                subprocesses.push_back(std::thread(threadSubprocess, std::ref(gm), j));
            }

            for(auto& subprocess : subprocesses) {
                subprocess.join();
            }

            if(gm.isTerminated()) {
                break;
            }

            gm.apply();

            gm.generate();

            if(i % 250 == 0) {
                gm.getDistributor().updateState();
            }

            end = std::chrono::system_clock::now();
            int timeSpent = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
            int timeToSleep = Config::STEP_TIMEOUT*1000 - timeSpent;
            if(timeToSleep > 0) {
                usleep(Config::STEP_TIMEOUT*1000);
            }

            i++;

            //std::cout << "step " << i << " finished" << std::endl;
        }

        end = std::chrono::system_clock::now();
        std::cout << "time spent: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
        std::cout << "process finished " << std::endl;
    }

    void threadSubprocess(GroupManager& gm, int id) {
        //std::cout << "subprocess " << id << " started" << std::endl;

        Cell* cell;
        Map& map = gm.getMap();
        Coords offset(1, 0);

        do {
            if(gm.cannotYield()) {
                break;
            }

            std::vector<Cell*> cells;

            gm.getReadableMutex().lock();
            for(int i=0; i<Config::CELLS_QUANTUM; i++) {
                cell = gm.yield();
                if(cell == nullptr) {
                    break;
                }
                cells.push_back(cell);
            }
            gm.getReadableMutex().unlock();
            //std::this_thread::yield();

            for(Cell* _cell : cells) {
                gm.process(_cell);
            }
        } while(true);

        //std::cout << std::endl;

        //std::cout << "subprocess " << id << " finished" << std::endl;
    }
}
