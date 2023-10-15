#ifndef SET_UTILITY_DOT_H
#define SET_UTILITY_DOT_H

#include <set>
#include "vectorUtility.h"

using namespace std;

namespace setUtil {

    template <typename T>
    ostream& operator << (ostream& out, const set<T>& s) {
        vecUtil::operator << (out, vector<T>(s.begin(), s.end()));
        return out;
    }

    template <typename T>
    set<T> set_union(const set<T>& s1, const set<T>& s2) {
        set<T> result;
        for (const T& element : s1) {
            result.insert(element);
        }
        for (const T& element : s2) {
            result.insert(element);
        }
        return result;
    }

    template <typename T>
    set<T> intersect(const set<T>& s1, const set<T>& s2) {
        set<T> result;
        for (const T& element : s1) {
            if (s2.count(element)) {
                result.insert(element);
            }
        }
        return result;
    }

    template <typename T>
    set<T> difference(const set<T>& full, const set<T>& remove) {
        set<T> result;
        for (const T& element : full) {
            if (!remove.count(element)) {
                result.insert(element);
            }
        }
        return result;
    }

    template <typename T>
    bool isSubset(const set<T>& sub, const set<T>& full, bool strict = false) {
        for (const T& element : sub) {
            if (!full.count(element)) {
                return false;
            }
        }
        return strict ? (sub.size() < full.size()) : true;
    }

    template <typename T>
    bool isSuperset(const set<T>& super, const set<T>& orig, bool strict = false) {
        return isSubset(orig, super, strict);
    }

}

#endif