#pragma once

#include <functional>
#include <map>
#include "coords.h"

namespace GenBrains {
    class Distributor
    {
    public:
        Distributor(double defaultState, const std::function<double(const Distributor&, double, double)>& stateUpdater);
        void addDistribution(const std::string name, const std::function<double(const Distributor&, const Coords&, int, int)> distribution);
        double getDistributionValue(const std::string name, const Coords& coords, int width, int height) const;
        double getState() const;
        void updateState();
    protected:
        double state;
        double arg;
        const std::function<double(const Distributor&, double, double)> stateUpdater;
        std::map<std::string, const std::function<double(const Distributor&, const Coords&, int, int)>> distributions;
    };
}
