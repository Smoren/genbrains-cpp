#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>
#include <utility>
#include <set>
#include <map>
#include <deque>


template <typename Collection>
std::string join(const Collection& collection, const std::string& delimiter);

template <typename First, typename Second>
std::ostream& operator <<(std::ostream& stream, const std::pair<First, Second>& p);

template <typename Key, typename Value>
std::ostream& operator <<(std::ostream& stream, const std::map<Key, Value>& m);

template <typename Value>
std::ostream& operator <<(std::ostream& stream, const std::vector<Value>& m);

template <typename Value>
std::ostream& operator <<(std::ostream& stream, const std::deque<Value>& m);

template <typename Value>
std::ostream& operator <<(std::ostream& stream, const std::set<Value>& m);


template <typename Collection>
std::string join(const Collection& collection, const std::string& delimiter) {
    bool isFirst = true;
    std::stringstream ss;
    for(auto& item : collection) {
        if(!isFirst) {
            ss << delimiter;
        } else {
            isFirst = false;
        }
        ss << item;
    }
    return ss.str();
}

template <typename First, typename Second>
std::ostream& operator <<(std::ostream& stream, const std::pair<First, Second>& p) {
    // требует подключения utility
    return stream << p.first << ": " << p.second;
}

template <typename Key, typename Value>
std::ostream& operator <<(std::ostream& stream, const std::map<Key, Value>& m) {
    return stream << "{" << join(m, ", ") << "}";
}

template <typename Value>
std::ostream& operator <<(std::ostream& stream, const std::vector<Value>& m) {
    return stream << "[" << join(m, ", ") << "]";
}

template <typename Value>
std::ostream& operator <<(std::ostream& stream, const std::set<Value>& m) {
    return stream << "(" << join(m, ", ") << ")";
}

template <typename Value>
std::ostream& operator <<(std::ostream& stream, const std::deque<Value>& m) {
    return stream << "[" << join(m, ", ") << "]";
}
