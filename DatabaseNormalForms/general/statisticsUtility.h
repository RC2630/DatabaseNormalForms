#ifndef STAT_UTIL_DOT_H
#define STAT_UTIL_DOT_H

#include "numberUtility.h"
#include "vectorUtility.h"
#include "abstractFunctions.h"
#include "setUtility.h"

#include <map>
#include <numeric>
#include <algorithm>

using namespace std;

// for functions that operate on vector<double>, they are more purely data-oriented, and work only on numerical data
// for functions that operate on vector<T>, they are more flexible with the type of data, but require you to define operators
// all functions that operate on vector<T> can work on vector<double> as well, since double has pre-defined operators <, ==, and +

namespace statUtil {

    // operator + must be defined for type T, and for any values t1, t2 of type T, t1 + t2 must produce a sum of type T as well
    // also, if T is a custom data type, such as a struct or class, a default constructor (with 0 arguments) must be publicly accessible
    template <typename T>
    T sum(const vector<T>& data) {
        return accumulate(data.begin(), data.end(), T());
    }

    double mean(const vector<double>& data);
    double median(const vector<double>& data);

    // operator < must be defined for type T, such that the data can be sorted
    template <typename T>
    T max(const vector<T>& data) {
        return *max_element(data.begin(), data.end());
    }

    // operator < must be defined for type T, such that the data can be sorted
    template <typename T>
    T min(const vector<T>& data) {
        return *min_element(data.begin(), data.end());
    }

    double range(const vector<double>& data);
    // popOrSample must be one of: "population", "sample"
    double variance(const vector<double>& data, const string& popOrSample);
    // popOrSample must be one of: "population", "sample"
    double stdDev(const vector<double>& data, const string& popOrSample);
    // popOrSample must be one of: "population", "sample"
    vector<double> scale(const vector<double>& unscaledData, const string& popOrSample);

    // operator < must be defined for type T, such that the data can be sorted
    template <typename T>
    map<T, int> frequencyTable(const vector<T>& data) {
        map<T, int> freqMap;
        for (const T& e : data) {
            freqMap[e]++;
        }
        return freqMap;
    }

    // operator < must be defined for type T, such that the data can be sorted
    template <typename T>
    T mode(const vector<T>& data) {
        map<T, int> freqMap = frequencyTable(data);
        return max_element(freqMap.begin(), freqMap.end(), [] (const pair<T, int>& p1, const pair<T, int>& p2) {
            return p1.second < p2.second;
        })->first;
    }

    // returns true if v1 is earlier in permutation order than v2
    // the permutation order is the order in the example below
    bool earlierInPermutationOrder(const vector<int>& v1, const vector<int>& v2);

    // generates permutations from 1 to n
    // example: generatePermutations(3) = {{1, 2, 3}, {1, 3, 2}, {2, 1, 3}, {2, 3, 1}, {3, 1, 2}, {3, 2, 1}}
    vector<vector<int>> generatePermutations(int n);

    // gives the next combination (see below)
    // example: nextCombination(5, {1, 2, 4}) = {1, 2, 5}; nextCombination(5, {1, 2, 5}) = {1, 3, 4}
    set<int> nextCombination(int n, const set<int>& currCombination);

    // generates all possible combinations of size k from the numbers 1 to n
    // example: generateCombinations(4, 3) = {{1, 2, 3}, {1, 2, 4}, {1, 3, 4}, {2, 3, 4}}
    vector<set<int>> generateCombinations(int n, int k);

    // generates all possible combinations of all sizes from 0 to n from the numbers 1 to n
    // example: generateCombinationsUpTo(4) = {
    //     {},
    //     {1}, {2}, {3}, {4},
    //     {1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4},
    //     {1, 2, 3}, {1, 2, 4}, {1, 3, 4}, {2, 3, 4},
    //     {1, 2, 3, 4}
    // }
    vector<set<int>> generateCombinationsUpTo(int n);

    // starting from "start" (>= 0) and ending at "end" (>= start), generate labels of all numbers with the same number of characters
    // example: generateNumberLabels(0, 100) = {"000", "001", ..., "098", "099", "100"}
    // example: generateNumberLabels(998, 1002) = {"0998", "0999", "1000", "1001", "1002"}
    vector<string> generateNumberLabels(int start, int end);

}

#endif