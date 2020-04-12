#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>
#include <utility>
#include <set>
#include <map>
#include <deque>


using namespace std;

template <typename Collection>
string join(const Collection& collection, const string& delimiter);

template <typename First, typename Second>
ostream& operator <<(ostream& stream, const pair<First, Second>& p);

template <typename Key, typename Value>
ostream& operator <<(ostream& stream, const map<Key, Value>& m);

template <typename Value>
ostream& operator <<(ostream& stream, const vector<Value>& m);



template <typename Collection>
string join(const Collection& collection, const string& delimiter) {
    bool isFirst = true;
    stringstream ss;
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
ostream& operator <<(ostream& stream, const pair<First, Second>& p) {
    // требует подключения utility
    return stream << p.first << ": " << p.second;
}

template <typename Key, typename Value>
ostream& operator <<(ostream& stream, const map<Key, Value>& m) {
    return stream << "{" << join(m, ", ") << "}";
}

template <typename Value>
ostream& operator <<(ostream& stream, const vector<Value>& m) {
    return stream << "[" << join(m, ", ") << "]";
}

template <typename Value>
ostream& operator <<(ostream& stream, const deque<Value>& m) {
    return stream << "[" << join(m, ", ") << "]";
}
