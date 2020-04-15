#pragma once

#include <vector>
#include <map>
#include <mutex>
#include "printer.h"


namespace GenBrains {
    template<typename ClusterMapItem> class ClusterMap {
    public:
        class iterator;
        friend class iterator;
        class iterator: public std::iterator<std::bidirectional_iterator_tag, ClusterMapItem, ptrdiff_t> {
            typename std::vector< std::map<unsigned long, ClusterMapItem> >::iterator itContainer;
            typename map<unsigned long, ClusterMapItem>::iterator itSubContainer;
            std::vector< std::map<unsigned long, ClusterMapItem> >* container;
            std::map<unsigned long, ClusterMapItem>* subContainer;
            public: iterator(
                std::vector< std::map<unsigned long, ClusterMapItem> >& _container,
                std::map<unsigned long, ClusterMapItem>& _subContainer,
                const typename std::vector< std::map<unsigned long, ClusterMapItem> >::iterator& _itContainer,
                const typename map<unsigned long, ClusterMapItem>::iterator& _itSubContainer
            ) : itContainer(_itContainer), itSubContainer(_itSubContainer), container(&_container), subContainer(&_subContainer) {
                if(itSubContainer == subContainer->end() && itContainer != --container->end()) {
                    itContainer = --container->end();
                    itSubContainer = itContainer->end();
                }
            }
            bool operator==(const iterator& x) const {
                return itSubContainer == x.itSubContainer;
            }
            bool operator!=(const iterator& x) const {
                return !(*this == x);
            }
            typename map<unsigned long, ClusterMapItem>::reference operator*() const {
                return *itSubContainer;
            }
            iterator &operator++() {
                ++itSubContainer;

                while(itSubContainer == subContainer->end()) {
                    ++itContainer;

                    if(itContainer != container->end()) {
                        itSubContainer = (*itContainer).begin();
                        subContainer = &(*itContainer);
                    } else {
                        break;
                    }
                }

                return *this;
            }

            iterator operator++(int) {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }
        };

        ClusterMap(unsigned long clustersCount, unsigned long clusterSize) : clustersCount(clustersCount), clusterSize(clusterSize) {
            if(!clustersCount || !clusterSize) {
                throw std::runtime_error("bad ClusterMap config");
            }
            data.resize(static_cast<unsigned long>(clustersCount));
            for(unsigned long i=0; i<clustersCount; i++) {
                mtxs.push_back(new std::mutex());
            }
        }

        ~ClusterMap() {
            for(auto& mtx : mtxs) {
                delete mtx;
            }
        }

        virtual void insert(std::pair<unsigned long, ClusterMapItem> item) {
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

        iterator find(unsigned long index) {
            unsigned long mapIndex = getMapIndex(index);
            try {
                auto& targetMap = data.at(mapIndex);
                return iterator(data, targetMap, data.begin()+mapIndex, targetMap.find(index));
            } catch(std::runtime_error e) {
                auto& targetMap = data.at(clustersCount-1);
                return iterator(data, targetMap, --data.end(), targetMap.end());
            }
        }

        ClusterMapItem at(unsigned long index) {
            return data[getMapIndex(index)].at(index);
        }

        unsigned long size() {
            unsigned long result = 0;
            for(auto& m : data) {
                result += m.size();
            }

            return result;
        }

        iterator begin() {
            auto containerIter = data.begin();
            for(unsigned long i=0; i<data.size(); i++) {
                if(data[i].size()) {
                    break;
                } else {
                    ++containerIter;
                }
            }

            if(containerIter == data.end()) {
                --containerIter;
            }

            auto& firstMap = (*containerIter);

            return iterator(data, firstMap, containerIter, firstMap.begin());
        }

        iterator end() {
            std::map<unsigned long, ClusterMapItem>& lastMap = data.at(data.size()-1);
            return iterator(data, lastMap, data.end(), lastMap.end());
        }

        std::map<unsigned long, ClusterMapItem> getData() const {
            std::map<unsigned long, ClusterMapItem> result;
            for(auto& cluster : data) {
                for(auto& item : cluster) {
                    result.insert(item);
                }
            }

            return result;
        }

        std::vector< std::map<int, ClusterMapItem> >& getContainer() {
            return data;
        }

        std::map<int, ClusterMapItem>& getCluster(unsigned long clusterId) {
            return data.at(clusterId);
        }
    protected:
        unsigned long clustersCount;
        unsigned long clusterSize;
        std::vector< std::map<unsigned long, ClusterMapItem> > data;
        std::vector<std::mutex*> mtxs;

        unsigned long getMapIndex(unsigned long index) {
            unsigned long mapIndex = index / clusterSize;
            if(mapIndex >= clustersCount) {
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
