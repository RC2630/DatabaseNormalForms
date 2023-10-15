#ifndef FD_DOT_H
#define FD_DOT_H

#include <iostream>
#include <set>

using namespace std;
using Attribute = string;

struct FunctionalDependency {

    set<Attribute> left;
    set<Attribute> right;

    FunctionalDependency(const set<Attribute>& left, const set<Attribute>& right);
    FunctionalDependency(const string& raw);

    bool operator < (const FunctionalDependency& other) const;
    bool operator == (const FunctionalDependency& other) const;

};

ostream& operator << (ostream& out, const FunctionalDependency& fd);
ostream& operator << (ostream& out, const set<FunctionalDependency>& fds);

namespace fd {

    set<FunctionalDependency> findAllFunctionalDependencies(const set<FunctionalDependency>& fds);
    set<FunctionalDependency> removeIrrelevantFDs(const set<FunctionalDependency>& fds, const set<Attribute>& atts);
    set<Attribute> closure(const set<Attribute>& atts, const set<FunctionalDependency>& fds);
    
}

#endif