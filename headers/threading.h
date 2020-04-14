#pragma once

#include <thread>
#include <mutex>
#include "groupmanager.h"


namespace GenBrains {
    void threadProcess(GroupManager& gm);
    void threadSubprocess(GroupManager& gm, int id);
    void threadApplyAdd(GroupManager& gm, int limit, int id);
    void threadApplyRemove(GroupManager& gm, int limit, int id);
}
