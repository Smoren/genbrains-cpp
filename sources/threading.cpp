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

        gm.getGroup().setHandler([&gm, &threadSteps](ClusterGroup<Cell>& cg, Cluster<Cell>& cluster) {
            unsigned long id = cluster.getId();

            while(!cg.isTerminated()) {
                for(auto* cell : cluster.getStorage()) {
                    gm.process(cell);
                }
                cg.finishPhaseBuffering();
                cluster.apply();
                cg.finishPhaseApplying();

                cluster.lockStorage();
                ++threadSteps;
                cluster.unlockStorage();

                if(threadSteps % (250*Config::THREADS) == 0) {
                    gm.getDistributor().updateState();
                }
            }
        });
        gm.getGroup().run();
    }
}
