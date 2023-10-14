#ifndef RELATION_DOT_H
#define RELATION_DOT_H

#include "FunctionalDependency.h"

#include <set>

using namespace std;

struct Relation {

    string name;
    set<Attribute> atts;
    set<FunctionalDependency> fds;
    bool printFDs = true;

    Relation(const string& name, const set<Attribute>& atts, const set<FunctionalDependency>& fds);
    Relation(const string& schema, const set<string>& rawFds);

    set<set<Attribute>> findAllKeys() const;
    void inheritFDsFrom(const Relation& parent);

    bool operator < (const Relation& other) const;
    bool operator == (const Relation& other) const;

};

ostream& operator << (ostream& out, const Relation& rel);

namespace rel {

    Relation getByName(const set<Relation>& rels, string name);
    set<Relation> readFromFile(const string& filename);

}

#endif