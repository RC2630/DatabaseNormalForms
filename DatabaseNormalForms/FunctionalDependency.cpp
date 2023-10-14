#include "FunctionalDependency.h"

#include "general/parseArguments.h"
#include "general/stringUtility.h"

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

set<FunctionalDependency> fd::findAllFunctionalDependencies(const set<FunctionalDependency>& fds) {
    // TODO
    return fds;
}