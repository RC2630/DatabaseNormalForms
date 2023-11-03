#include "FunctionalDependency.h"

#include "general/parseArguments.h"
#include "general/stringUtility.h"
#include "general/setUtility.h"
#include "general/ansi_codes.h"
#include "general/mapUtility.h"
#include "general/abstractFunctions.h"
#include "general/statisticsUtility.h"

FunctionalDependency::FunctionalDependency(const set<Attribute>& left, const set<Attribute>& right)
: left(left), right(right)
{ }

FunctionalDependency::FunctionalDependency(const string& raw) {

    string arrowSep = " -> ";
    if (strUtil::contains(raw, "@")) {
        throw runtime_error(raw); // relation pointer: need to get back console input line with e.what()
    } else if (!strUtil::contains(raw, arrowSep)) {
        throw invalid_argument("raw FD does not contain an arrow");
    }

    int indexArrow = raw.find(arrowSep);
    string leftRaw = raw.substr(0, indexArrow);
    string rightRaw = raw.substr(indexArrow + arrowSep.size());

    vector<string> left = parse::parseAllArguments(leftRaw, true, ',');
    for (string& att : left) {
        if (strUtil::beginsWith(att, " ")) {
            att = att.substr(1);
        }
    }

    vector<string> right = parse::parseAllArguments(rightRaw, true, ',');
    for (string& att : right) {
        if (strUtil::beginsWith(att, " ")) {
            att = att.substr(1);
        }
    }

    this->left = set<Attribute>(left.begin(), left.end());
    this->right = set<Attribute>(right.begin(), right.end());

}

bool FunctionalDependency::operator < (const FunctionalDependency& other) const {
    return tie(left, right) < tie(other.left, other.right);
}

bool FunctionalDependency::operator == (const FunctionalDependency& other) const {
    return tie(left, right) == tie(other.left, other.right);
}

ostream& operator << (ostream& out, const FunctionalDependency& fd) {

    string output;
    for (Attribute att : fd.left) {
        output += att + ", ";
    }
    output = output.substr(0, output.size() - 2) + " -> ";

    for (Attribute att : fd.right) {
        output += att + ", ";
    }
    output = output.substr(0, output.size() - 2);

    out << output;
    return out;

}

ostream& operator << (ostream& out, const set<FunctionalDependency>& fds) {
    if (fds.empty()) {
        out << ANSI_RED << "\nThere are no functional dependencies to display.\n" << ANSI_NORMAL;
    } else {
        out << "\n";
        for (const FunctionalDependency& fd : fds) {
            out << fd << "\n";
        }
    }
    return out;
}

set<FunctionalDependency> fd::findAllFunctionalDependencies(const set<FunctionalDependency>& fds, bool full) {

    set<FunctionalDependency> resultStep1, resultStep2;

    // part 1: find implicit FD's (also removes trivial FD's)

    // case A: optimized for performance, but may miss some implicit FD's
    if (!full) {
        for (const FunctionalDependency& fd : fds) {
            set<Attribute> closureAtts = closure(fd.left, fds);
            set<Attribute> newRight = setUtil::difference(closureAtts, fd.left);
            if (newRight.size() > 0) {
                resultStep1.insert(FunctionalDependency(fd.left, newRight));
            }
        }
    }

    // case B: optimized for completeness, but may take a long time to run
    if (full) {
        
        set<Attribute> allAttsInFDs;
        for (const FunctionalDependency& fd : fds) {
            allAttsInFDs.insert(fd.left.begin(), fd.left.end());
            allAttsInFDs.insert(fd.right.begin(), fd.right.end());
        }

        vector<Attribute> allAttsInFDsVec = setUtil::setToVector(allAttsInFDs);
        vector<set<int>> combs = statUtil::generateCombinationsUpTo(allAttsInFDs.size());

        for (const set<int>& comb : combs) {

            set<Attribute> currAtts = setUtil::vectorToSet(
                absFunc::map_f<int, Attribute>(setUtil::setToVector(comb), [&allAttsInFDsVec] (const int& index) {
                    return allAttsInFDsVec.at(index - 1);
                })
            );

            set<Attribute> closureAtts = closure(currAtts, fds);
            set<Attribute> newRight = setUtil::difference(closureAtts, currAtts);
            if (newRight.size() > 0) {
                resultStep1.insert(FunctionalDependency(currAtts, newRight));
            }

        }

    }

    // part 2: split multiple-right-side FD's into multiple FD's
    for (const FunctionalDependency& fd : resultStep1) {
        for (const Attribute& att : fd.right) {
            resultStep2.insert(FunctionalDependency(fd.left, {att}));
        }
    }

    return resultStep2;

}

set<FunctionalDependency> fd::removeIrrelevantFDs(const set<FunctionalDependency>& fds, const set<Attribute>& atts) {
    set<FunctionalDependency> relevantFDs;
    for (const FunctionalDependency& fd : fds) {
        if (setUtil::isSubset(setUtil::setUnion(fd.left, fd.right), atts)) {
            relevantFDs.insert(fd);
        }
    }
    return relevantFDs;
}

set<Attribute> fd::closure(const set<Attribute>& atts, const set<FunctionalDependency>& fds) {
    
    set<Attribute> closureAtts = atts;
    bool hasChanged = true;

    while (hasChanged) {

        hasChanged = false;
        for (const FunctionalDependency& fd : fds) {
            if (setUtil::isSubset(fd.left, closureAtts)) {

                int sizeBeforeInsert = closureAtts.size();
                closureAtts.insert(fd.right.begin(), fd.right.end());
                
                if (sizeBeforeInsert < closureAtts.size()) {
                    hasChanged = true;
                }

            }
        }

    }

    return closureAtts;

}

set<FunctionalDependency> fd::minimalCover(const set<FunctionalDependency>& fds) {
    
    // step 1: take each fd and split its right side attributes into multiple functional dependencies

    set<FunctionalDependency> step1;
    for (const FunctionalDependency& fd : fds) {
        for (const Attribute& att : fd.right) {
            step1.insert(FunctionalDependency(fd.left, {att}));
        }
    }

    // step 2: attempt to simplify the left side of each FD by removing redundant attributes from it

    map<int, FunctionalDependency> step2 = mapUtil::makeMap<int, FunctionalDependency>(
        absFunc::buildList<int>(step1.size(), [] (int n) {
            return n;
        }),
        setUtil::setToVector(step1)
    );

    auto canAttributeBeRemoved = [&step2] (const FunctionalDependency& fd, const Attribute& att) -> bool {
        set<Attribute> closureAtts = fd::closure(
            setUtil::difference(fd.left, {att}),
            setUtil::vectorToSet(mapUtil::getValues(step2))
        );
        return closureAtts.count(att);
    };

    function<void (FunctionalDependency&)> simplifyLeftSide;
    simplifyLeftSide = [canAttributeBeRemoved, &simplifyLeftSide] (FunctionalDependency& fd) -> void {

        // base case
        if (fd.left.size() == 1) {
            return;
        }

        // recursive case
        for (const Attribute& att : fd.left) {
            if (canAttributeBeRemoved(fd, att)) {
                fd = FunctionalDependency(setUtil::difference(fd.left, {att}), fd.right);
                simplifyLeftSide(fd);
                return;
            }
        }

    };

    for (int i : mapUtil::getKeys(step2)) {
        simplifyLeftSide(step2.at(i));
    }

    // step 3: attempt to remove redundant functional dependencies

    map<int, pair<FunctionalDependency, bool>> step3;
    for (const auto& entry : step2) {
        step3.insert({entry.first, {entry.second, false}}); // the bool is whether or not this FD has been removed
    }

    auto onlyNonRemovedFDs = [&step3] () -> set<FunctionalDependency> {

        vector<pair<FunctionalDependency, bool>> filtered = absFunc::filter_f<pair<FunctionalDependency, bool>>(
            mapUtil::getValues(step3), [] (const pair<FunctionalDependency, bool>& fd) {
                return !fd.second; // filter for only the not removed FD's
            }
        );

        return setUtil::vectorToSet(absFunc::map_f<pair<FunctionalDependency, bool>, FunctionalDependency>(
            filtered, [] (const pair<FunctionalDependency, bool>& fdPair) {
                return fdPair.first;
            }
        ));

    };

    auto attemptRedundantFDremoval = [&step3, onlyNonRemovedFDs] (int index) -> void {

        FunctionalDependency& fd = step3.at(index).first;
        step3.at(index).second = true; // pretend that we have removed the FD

        set<Attribute> closureAtts = fd::closure(fd.left, onlyNonRemovedFDs());
        if (!setUtil::isSuperset(closureAtts, fd.right)) {
            step3.at(index).second = false; // not actually redundant, put it back
        }

    };

    for (int i : mapUtil::getKeys(step3)) {
        attemptRedundantFDremoval(i);
    }

    return onlyNonRemovedFDs();

}