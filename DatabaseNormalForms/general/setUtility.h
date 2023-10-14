#ifndef SET_UTILITY_DOT_H
#define SET_UTILITY_DOT_H

#include <set>

using namespace std;

namespace setUtil {

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

}

#endif