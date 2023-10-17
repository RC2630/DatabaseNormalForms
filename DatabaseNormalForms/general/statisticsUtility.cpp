#include "statisticsUtility.h"

double statUtil::mean(const vector<double>& data) {
    return sum(data) / data.size();
}

double statUtil::median(const vector<double>& data) {
    vector<double> copy = data;
    sort(copy.begin(), copy.end());
    if (copy.size() % 2 == 1) {
        return copy.at(copy.size() / 2);
    } else {
        return mean({copy.at(copy.size() / 2), copy.at(copy.size() / 2 - 1)});
    }
}

double statUtil::range(const vector<double>& data) {
    return max(data) - min(data);
}

// popOrSample must be one of: "population", "sample"
double statUtil::variance(const vector<double>& data, const string& popOrSample) {
    int denominator = (popOrSample == "population") ? data.size() : (data.size() - 1);
    double dataMean = mean(data);
    double sumOfSquaredDists = accumulate(data.begin(), data.end(), 0.0, [dataMean](double rsf, double curr) {
        return rsf + pow(curr - dataMean, 2);
        });
    return sumOfSquaredDists / denominator;
}

// popOrSample must be one of: "population", "sample"
double statUtil::stdDev(const vector<double>& data, const string& popOrSample) {
    return sqrt(variance(data, popOrSample));
}

// popOrSample must be one of: "population", "sample"
vector<double> statUtil::scale(const vector<double>& unscaledData, const string& popOrSample) {
    double dataMean = mean(unscaledData);
    double dataSD = stdDev(unscaledData, popOrSample);
    vector<double> scaledData;
    for (double n : unscaledData) {
        scaledData.push_back((n - dataMean) / dataSD);
    }
    return scaledData;
}

// returns true if v1 is earlier in permutation order than v2
// the permutation order is the order in the example below
bool statUtil::earlierInPermutationOrder(const vector<int>& v1, const vector<int>& v2) {
    for (int i = 0; i < v1.size(); i++) {
        if (v1.at(i) < v2.at(i)) {
            return true;
        } else if (v1.at(i) > v2.at(i)) {
            return false;
        }
    }
    throw runtime_error("all elements of v1 and v2 appear to be equal");
}

// generates permutations from 1 to n
// example: generatePermutations(3) = {{1, 2, 3}, {1, 3, 2}, {2, 1, 3}, {2, 3, 1}, {3, 1, 2}, {3, 2, 1}}
vector<vector<int>> statUtil::generatePermutations(int n) {
    if (n == 0) {
        return {};
    } else if (n == 1) {
        return {{1}};
    } else {
        vector<vector<int>> prev = generatePermutations(n - 1);
        vector<vector<int>> result;
        for (const vector<int>& perm : prev) {
            for (int i = 0; i <= perm.size(); i++) {
                vector<int> temp = perm;
                vecUtil::insertAtPos(temp, i, n);
                int indexToInsert = 0;
                for (int r = 0; r < result.size(); r++) {
                    if (earlierInPermutationOrder(result.at(r), temp)) {
                        indexToInsert++;
                    } else {
                        break;
                    }
                }
                vecUtil::insertAtPos(result, indexToInsert, temp);
            }
        }
        return result;
    }
}

// gives the next combination (see below)
// example: nextCombination(5, {1, 2, 4}) = {1, 2, 5}; nextCombination(5, {1, 2, 5}) = {1, 3, 4}
set<int> statUtil::nextCombination(int n, const set<int>& currCombination) {

    vector<int> combination = setUtil::setToVector(currCombination);
    int indexToIncrement = combination.size() - 1;
    int ifEqualThenKeepGoing = n;

    while (combination.at(indexToIncrement) == ifEqualThenKeepGoing) {
        indexToIncrement--;
        ifEqualThenKeepGoing--;
        if (indexToIncrement == -1) {
            throw runtime_error("last combination reached");
        }
    }

    combination.at(indexToIncrement)++;
    for (int i = indexToIncrement + 1; i < combination.size(); i++) {
        combination.at(i) = combination.at(i - 1) + 1;
    }
    
    return setUtil::vectorToSet(combination);

}

// generates all possible combinations of size k from the numbers 1 to n
// example: generateCombinations(4, 3) = {{1, 2, 3}, {1, 2, 4}, {1, 3, 4}, {2, 3, 4}}
vector<set<int>> statUtil::generateCombinations(int n, int k) {

    set<int> firstCombination = setUtil::vectorToSet(absFunc::buildList<int>(k, [] (int i) {
        return i + 1;
    }));

    set<int> lastCombination = setUtil::vectorToSet(absFunc::buildList_f<int>(k, [n, k] (int i) {
        return i + n - k + 1;
    }));

    vector<set<int>> result = {firstCombination};
    set<int> currCombination = firstCombination;

    while (currCombination != lastCombination) {
        currCombination = nextCombination(n, currCombination);
        result.push_back(currCombination);
    }

    return result;

}

// generates all possible combinations of all sizes from 0 to n from the numbers 1 to n
// example: generateCombinationsUpTo(4) = {
//     {},
//     {1}, {2}, {3}, {4},
//     {1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4},
//     {1, 2, 3}, {1, 2, 4}, {1, 3, 4}, {2, 3, 4},
//     {1, 2, 3, 4}
// }
vector<set<int>> statUtil::generateCombinationsUpTo(int n) {
    vector<set<int>> result;
    for (int i = 0; i <= n; i++) {
        result = vecUtil::concatenate<set<int>>({result, generateCombinations(n, i)});
    }
    return result;
}

// starting from "start" (>= 0) and ending at "end" (>= start), generate labels of all numbers with the same number of characters
// example: generateNumberLabels(0, 100) = {"000", "001", ..., "098", "099", "100"}
// example: generateNumberLabels(998, 1002) = {"0998", "0999", "1000", "1001", "1002"}
vector<string> statUtil::generateNumberLabels(int start, int end) {
    int numDigits = to_string(end).size();
    vector<string> result;
    for (int i = start; i <= end; i++) {
        string s = to_string(i);
        result.push_back(string(numDigits - s.size(), '0') + s);
    }
    return result;
}