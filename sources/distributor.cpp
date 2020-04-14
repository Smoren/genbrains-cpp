#include <sstream>
#include "headers/distributor.h"

namespace GenBrains {
    Distributor::Distributor(
        double defaultState, const std::function<double(const Distributor&, double, double)>& stateUpdater
    ) : state(defaultState), arg(0), stateUpdater(stateUpdater) {

    }

    void Distributor::addDistribution(const std::string name, const std::function<double(const Distributor&, const Coords&, int, int)> distribution) {
        distributions.insert({name, distribution});
    }

    double Distributor::getDistributionValue(const std::string name, const Coords& coords, int width, int height) const {
        if(distributions.find(name) == distributions.end()) {
            std::stringstream ss;
            ss << "distribution is not exist (name: " << name << ")";
            throw std::runtime_error(ss.str());
        }
        return distributions.at(name)(*this, coords, width, height);
    }

    double Distributor::getState() const {
        return state;
    }

    void Distributor::updateState() {
        state = stateUpdater(*this, arg++, state);
        std::cout << "state: " << state << std::endl;
    }
}
