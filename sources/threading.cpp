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

        gm.getGroup().setHandler([&gm, &threadSteps, &stepCounterMutex](ClusterGroup<Cell>& cg, Cluster<Cell>& cluster) {
            int divider = 250*Config::THREADS;

            while(!cg.isTerminated()) {
                for(auto* cell : cluster.getStorage()) {
                    gm.process(cell);
                }

                cg.finishPhaseBuffering();
                cluster.apply();
                cg.finishPhaseApplying();

                stepCounterMutex.lock();
                ++threadSteps;
                if(threadSteps % divider == 0) {
                    gm.getDistributor().updateState();

                    std::cout << "clusters: ";
                    for(auto* cl : gm.getGroup().getClusters()) {
                        std::cout << cl->getStorage().size() << " ";
                    }
                    std::cout << std::endl;
                }
                stepCounterMutex.unlock();
            }
        });
        gm.getGroup().run();

        while(!gm.getGroup().isTerminated()) {
            std::this_thread::yield();
        }
    }
}
