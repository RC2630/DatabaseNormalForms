#include "FunctionalDependency.h"

#include "general/parseArguments.h"
#include "general/stringUtility.h"
#include "general/setUtility.h"
#include "general/ansi_codes.h"

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

set<FunctionalDependency> fd::findAllFunctionalDependencies(const set<FunctionalDependency>& fds) {

    set<FunctionalDependency> resultStep1, resultStep2;

    // part 1: find implicit FD's (also removes trivial FD's)
    for (const FunctionalDependency& fd : fds) {
        set<Attribute> closureAtts = closure(fd.left, fds);
        set<Attribute> newRight = setUtil::difference(closureAtts, fd.left);
        if (newRight.size() > 0) {
            resultStep1.insert(FunctionalDependency(fd.left, newRight));
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