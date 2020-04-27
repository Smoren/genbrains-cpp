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
        int threadSteps = 0;
        std::mutex stepCounterMutex;

        gm.getGroup().setOnFinishPhaseHandler([&gm, &threadSteps, &stepCounterMutex](ClusterGroup<Cell>& cg, bool phase) {
            if(!phase) {
                return;
            }
            stepCounterMutex.lock();
            ++threadSteps;
            if(threadSteps % 250 == 0) {
                gm.getDistributor().updateState();

                std::cout << "clusters: ";
                for(auto* cl : gm.getGroup().getClusters()) {
                    std::cout << cl->getStorage().size() << " ";
                }
                std::cout << std::endl;
            }
            stepCounterMutex.unlock();
        });

        gm.getGroup().setThreadHandler([&gm](ClusterGroup<Cell>& cg, Cluster<Cell>& cluster) {
            while(!cg.isTerminated()) {
                for(auto* cell : cluster.getStorage()) {
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
