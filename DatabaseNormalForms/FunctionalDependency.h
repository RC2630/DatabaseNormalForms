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

};

ostream& operator << (ostream& out, const FunctionalDependency& fd);

namespace fd {

    // TODO

}

#endif