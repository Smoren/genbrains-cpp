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
        gm.getGroup().setHandler([&gm](ClusterGroup<Cell>& cg, Cluster<Cell>& cluster) {
            unsigned long id = cluster.getId();

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
    }
}
