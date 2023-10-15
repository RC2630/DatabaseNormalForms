#ifndef SET_UTILITY_DOT_H
#define SET_UTILITY_DOT_H

#include <set>
#include <sstream>

#include "vectorUtility.h"

using namespace std;

namespace setUtil {

    template <typename T>
    ostream& operator << (ostream& out, const set<T>& s) {
        stringstream sout;
        vecUtil::operator << (sout, vector<T>(s.begin(), s.end()));
        out << "{" << sout.str().substr(1, sout.str().size() - 2) << "}";
        return out;
    }

    template <typename T>
    set<T> setUnion(const set<T>& s1, const set<T>& s2) {
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
    set<T> setUnion(const set<set<T>>& sets) {
        if (sets.empty()) {
            return {};
        }
        set<T> result = *sets.begin();
        for (const set<T>& s : sets) {
            result = setUnion(result, s);
        }
        return result;
    }

    template <typename T>
    set<T> setUnion(const vector<set<T>>& sets) {
        set<set<T>> ssets(sets.begin(), sets.end());
        return setUnion(ssets);
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
    set<T> intersect(const set<set<T>>& sets) {
        if (sets.empty()) {
            return {};
        }
        set<T> result = *sets.begin();
        for (const set<T>& s : sets) {
            result = intersect(result, s);
        }
        return result;
    }

    template <typename T>
    set<T> intersect(const vector<set<T>>& sets) {
        set<set<T>> ssets(sets.begin(), sets.end());
        return intersect(ssets);
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

    template<typename T>
    set<T> difference(const set<T>& full, const set<set<T>>& removeSets) {
        set<T> result = full;
        for (const set<T>& s : removeSets) {
            result = difference(result, s);
        }
        return result;
    }

    template<typename T>
    set<T> difference(const set<T>& full, const vector<set<T>>& removeSets) {
        set<set<T>> removeSsets(removeSets.begin(), removeSets.end());
        return difference(full, removeSsets);
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

    template <typename T>
    set<T> vectorToSet(const vector<T>& v) {
        return set<T>(v.begin(), v.end());
    }

    template <typename T>
    vector<T> setToVector(const set<T>& s) {
        return vector<T>(s.begin(), s.end());
    }

}

#endif