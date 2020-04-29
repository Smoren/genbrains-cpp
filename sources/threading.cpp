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

using namespace Smoren::ThreadSafeContainers;

namespace GenBrains {
    void threadProcess(GroupManager& gm) {
        int threadSteps = 0;
        int fullSize = 0;
        std::mutex stepCounterMutex;
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();

        gm.getGroup().setOnFinishPhaseHandler([&gm, &threadSteps, &stepCounterMutex, &start, &end, &fullSize](ClusterGroup<Cell>& cg, bool phase) {
            if(!phase) {
                return;
            }
            stepCounterMutex.lock();
            ++threadSteps;
            if(threadSteps % 250 == 0) {
                gm.getDistributor().updateState();

                fullSize = 0;

                std::cout << "clusters: ";
                for(auto* cl : gm.getGroup().getClusters()) {
                    fullSize += cl->getStorage().size();
                    std::cout << cl->getStorage().size() << " ";
                }
                std::cout << std::endl;
                std::cout << "total: " << fullSize << std::endl;

                end = std::chrono::system_clock::now();
                auto spent = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
                std::cout << "time spent: " << spent << std::endl;
                std::cout << "perfomance index: " << (static_cast<float>(fullSize)/static_cast<float>(spent)) << std::endl;
                start = end;

                // TODO this_thread::sleap_for();

                std::cout << "=========" << std::endl;
            }

            stepCounterMutex.unlock();
        });

        gm.getGroup().setThreadHandler([&gm](ClusterGroup<Cell>& cg, Cluster<Cell>& cluster) {
            while(!cg.isTerminated()) {
                for(auto* cell : cluster) {
                    gm.process(cell);
                }
                cg.finishPhaseBuffering();

                cluster.apply();
                cg.finishPhaseApplying();
            }
        });
        gm.getGroup().run();

        while(!gm.getGroup().isTerminated()) {
            std::this_thread::yield();
        }
    }
}
