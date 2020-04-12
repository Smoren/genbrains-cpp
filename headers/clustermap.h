#pragma once

#include <vector>
#include <map>
#include <mutex>
#include "printer.h"


namespace GenBrains {
    template <typename ClusterMapItem>
    class ClusterMap
    {
    public:
        ClusterMap(unsigned long clusters, unsigned long clusterSize) : clusters(clusters), clusterSize(clusterSize) {
            if(!clusters || !clusterSize) {
                throw std::runtime_error("bad ClusterMap config");
            }
            data.resize(static_cast<unsigned long>(clusters));
            for(unsigned long i=0; i<clusters; i++) {
                mtxs.push_back(new std::mutex());
            }
        }

        ~ClusterMap() {
            for(auto& cluster : data) {
                for(auto& item : cluster) {
                    delete item.second;
                }
            }
            for(auto& mtx : mtxs) {
                delete mtx;
            }
        }

        void insert(std::pair<unsigned long, ClusterMapItem*> item) {
            auto mapIndex = getMapIndex(item.first);
            mtxs[mapIndex]->lock();
            data[mapIndex].insert(item);
            mtxs[mapIndex]->unlock();
        }

        void erase(unsigned long index) {
            auto mapIndex = getMapIndex(index);
            mtxs[mapIndex]->lock();
            data[mapIndex].erase(index);
            mtxs[mapIndex]->unlock();
        }

        ClusterMapItem* at(unsigned long index) {
            return data[getMapIndex(index)].at(index);
        }

        std::map<unsigned long, ClusterMapItem*> getData() const {
            std::map<unsigned long, ClusterMapItem*> result;
            for(auto& cluster : data) {
                for(auto& item : cluster) {
                    result.insert(item);
                }
            }

            return result;
        }
    protected:
        unsigned long clusters;
        unsigned long clusterSize;
        std::vector< std::map<unsigned long, ClusterMapItem*> > data;
        std::vector<std::mutex*> mtxs;

        unsigned long getMapIndex(unsigned long index) {
            unsigned long mapIndex = static_cast<unsigned long>(index) / clusterSize;
            if(mapIndex >= clusters) {
                throw std::runtime_error("map index is out of range");
            }

            return mapIndex;
        }
    };

    template <typename ClusterMapItemPointer>
    std::ostream& operator <<(std::ostream& stream, const ClusterMap<ClusterMapItemPointer>& item) {
        return stream << item.getData();
    }
}

