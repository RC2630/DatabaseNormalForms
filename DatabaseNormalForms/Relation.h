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

    Relation(const string& name, const set<Attribute>& atts, const set<FunctionalDependency>& fds, bool preprocess = true);
    Relation(const string& schema, const set<string>& rawFds, bool preprocess = true);

    set<set<Attribute>> findAllKeys() const;
    void inheritFDsFrom(const Relation& parent, bool removeIrrelevant = true);
    bool isDecompLossless(const set<Relation>& decomps) const;
    bool isSuperkey(const set<Attribute>& atts) const;
    bool isKey(const set<Attribute>& atts) const;
    set<set<Attribute>> findAllSuperkeys() const;

    bool violatesBCNF(const FunctionalDependency& fd) const;
    bool isInBCNF() const;
    set<Relation> decompBCNF() const;
    bool violates3NF(const FunctionalDependency& fd) const;
    bool isIn3NF() const;
    set<Relation> decomp3NFlosslessJoin() const;

    bool operator < (const Relation& other) const;
    bool operator == (const Relation& other) const;

private:

    set<Relation> decompBCNFhelper() const;
    
};

ostream& operator << (ostream& out, const Relation& rel);
ostream& operator << (ostream& out, const set<Relation>& rels);

namespace rel {

    Relation getByName(const set<Relation>& rels, string name);
    set<Relation> readFromFile(const string& filename, bool preprocess = true);
    set<Relation> removeRedundantRelations(const set<Relation>& rels);

}

#endif