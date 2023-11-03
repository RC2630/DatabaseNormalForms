#include "Relation.h"

#include "general/stringUtility.h"
#include "general/parseArguments.h"
#include "general/ansi_codes.h"
#include "general/file.h"
#include "general/mapUtility.h"
#include "general/setUtility.h"
#include "general/abstractFunctions.h"
#include "general/statisticsUtility.h"

#include <queue>

Relation::Relation(const string& name, const set<Attribute>& atts, const set<FunctionalDependency>& fds, bool preprocess, bool full, bool printFDs)
: name(name), atts(atts), fullyFindImps(full), printFDs(printFDs) {
    if (preprocess) {
        this->fds = fd::findAllFunctionalDependencies(fds, full);
        this->fds = fd::removeIrrelevantFDs(this->fds, this->atts);
    } else {
        this->fds = fds;
    }
}

Relation::Relation(const string& schema, const set<string>& rawFds, bool preprocess, bool full, bool printFDs)
: fullyFindImps(full), printFDs(printFDs) {
    
    this->name = strUtil::removeAllAfterChar(schema, '(');
    string raw = strUtil::removeAllBeforeChar(schema, '(', true);
    raw = raw.substr(0, raw.size() - 1);

    vector<string> atts = parse::parseAllArguments(raw, true, ',');
    for (string& att : atts) {
        if (strUtil::beginsWith(att, " ")) {
            att = att.substr(1);
        }
    }
    this->atts = set<Attribute>(atts.begin(), atts.end());

    for (const string& rawFd : rawFds) {
        this->fds.insert(FunctionalDependency(rawFd));
    }

    if (preprocess) {
        this->fds = fd::findAllFunctionalDependencies(this->fds, full);
        this->fds = fd::removeIrrelevantFDs(this->fds, this->atts);
    }

}

set<set<Attribute>> Relation::findAllKeys() const {

    set<Attribute> left, middle;

    auto appearsOnLeft = [this] (Attribute att) -> bool {
        for (const FunctionalDependency& fd : this->fds) {
            if (fd.left.count(att)) {
                return true;
            }
        }
        return false;
    };

    auto appearsOnRight = [this] (Attribute att) -> bool {
        for (const FunctionalDependency& fd : this->fds) {
            if (fd.right.count(att)) {
                return true;
            }
        }
        return false;
    };

    auto belongsToLeft = [this, appearsOnLeft, appearsOnRight] (Attribute att) -> bool {
        return !appearsOnRight(att);
    };

    auto belongsToMiddle = [this, appearsOnLeft, appearsOnRight] (Attribute att) -> bool {
        return appearsOnLeft(att) && appearsOnRight(att);
    };

    for (const Attribute& att : this->atts) {
        if (belongsToLeft(att)) {
            left.insert(att);
        } else if (belongsToMiddle(att)) {
            middle.insert(att);
        }
    }

    set<set<Attribute>> keys;
    queue<set<Attribute>> toTry;
    toTry.push(left);

    auto pushAllNextLayer = [this, &toTry, &middle] (const set<Attribute>& currTry) -> void {
        set<Attribute> potentialToAdds = setUtil::difference(middle, currTry);
        for (const Attribute& att : potentialToAdds) {
            toTry.push(setUtil::setUnion(currTry, {att}));
        }
    };

    while (!toTry.empty()) {
        set<Attribute> currTry = toTry.front();
        toTry.pop();
        if (this->isKey(currTry)) {
            keys.insert(currTry);
        } else {
            pushAllNextLayer(currTry);
        }
    }

    return keys;

}

void Relation::inheritFDsFrom(const Relation& parent, bool removeIrrelevant) {
    this->fds = parent.fds;
    if (removeIrrelevant) {
        this->fds = fd::removeIrrelevantFDs(this->fds, this->atts);
    }
}

bool Relation::isDecompLossless(const set<Relation>& decomps) const {

    if (decomps.size() != 2) {
        throw invalid_argument("input size must be 2");
    }

    set<Attribute> isecAtts = setUtil::intersect(absFunc::map_f<Relation, set<Attribute>>(setUtil::setToVector(decomps), [] (const Relation& rel) {
        return rel.atts;
    }));
    set<Attribute> closureAtts = fd::closure(isecAtts, this->fds);

    for (const Relation& rel : decomps) {
        if (setUtil::isSubset(rel.atts, closureAtts)) {
            return true;
        }
    }

    return false;

}

bool Relation::isSuperkey(const set<Attribute>& atts) const {
    set<Attribute> closureAtts = fd::closure(atts, this->fds);
    return closureAtts == this->atts;
}

bool Relation::isKey(const set<Attribute>& atts) const {

    if (!isSuperkey(atts)) {
        return false;
    }

    for (const Attribute& att : atts) {
        set<Attribute> attsRemove1 = atts;
        attsRemove1.erase(att);
        if (isSuperkey(attsRemove1)) {
            return false;
        }
    }

    return true;

}

set<set<Attribute>> Relation::findAllSuperkeys() const {

    set<set<Attribute>> keys = this->findAllKeys();
    set<set<Attribute>> superkeys;

    for (const set<Attribute>& key : keys) {

        vector<Attribute> notInKey = setUtil::setToVector(setUtil::difference(this->atts, key));
        vector<set<int>> combinations = statUtil::generateCombinationsUpTo(notInKey.size());
        vector<set<Attribute>> combAtts = absFunc::map_f<set<int>, set<Attribute>>(combinations, [&notInKey] (const set<int>& comb) {
            return setUtil::vectorToSet(absFunc::map_f<int, Attribute>(setUtil::setToVector(comb), [&notInKey] (int index) {
                return notInKey.at(index - 1);
            }));
        });

        for (const set<Attribute>& currCombAtts : combAtts) {
            superkeys.insert(setUtil::setUnion(key, currCombAtts));
        }

    }

    return superkeys;

}

bool Relation::violatesBCNF(const FunctionalDependency& fd) const {
    return !isSuperkey(fd.left);
}

bool Relation::isInBCNF() const {
    for (const FunctionalDependency& fd : this->fds) {
        if (violatesBCNF(fd)) {
            return false;
        }
    }
    return true;
}

set<Relation> Relation::decompBCNF() const {

    vector<Relation> decomp = setUtil::setToVector(rel::removeRedundantRelations(decompBCNFhelper()));
    vector<string> labels = statUtil::generateNumberLabels(1, decomp.size());

    if (decomp.size() == 1) {
        decomp.front().name = this->name;
        return setUtil::vectorToSet(decomp);
    }

    int labelIndex = 0;
    for (Relation& rel : decomp) {
        rel.name = this->name + "." + labels.at(labelIndex++);
    }

    return setUtil::vectorToSet(decomp);

}

set<Relation> Relation::decompBCNFhelper() const {

    for (const FunctionalDependency& fd : this->fds) {
        if (violatesBCNF(fd)) {
            Relation newRelFromFd(this->name, setUtil::setUnion(fd.left, fd.right), this->fds, true, this->fullyFindImps, this->printFDs);
            Relation newRelFromRest(this->name, setUtil::difference(this->atts, fd.right), this->fds, true, this->fullyFindImps, this->printFDs);
            return setUtil::setUnion(newRelFromFd.decompBCNFhelper(), newRelFromRest.decompBCNFhelper());
        }
    }

    // at this point, no FD's violate BCNF
    return {*this};

}

bool Relation::violates3NF(const FunctionalDependency& fd) const {
    bool leftIsSuperkey = this->isSuperkey(fd.left);
    bool rightIsPartOfKey = absFunc::ormap_f<set<Attribute>>(setUtil::setToVector(this->findAllKeys()), [&fd] (const set<Attribute>& key) {
        return setUtil::isSubset(fd.right, key);
    });
    return !leftIsSuperkey && !rightIsPartOfKey;
}

bool Relation::isIn3NF() const {
    for (const FunctionalDependency& fd : this->fds) {
        if (violates3NF(fd)) {
            return false;
        }
    }
    return true;
}

set<Relation> Relation::decomp3NFlosslessJoin() const {

    if (this->isIn3NF()) {
        return {*this};
    }

    set<Relation> decomp;
    set<FunctionalDependency> minCov = fd::minimalCover(this->fds);

    // pick an FD that violates 3NF, and decompose all the way down to BCNF on that FD
    for (const FunctionalDependency& fd : this->fds) {
        if (this->violates3NF(fd)) {
            Relation newRelFromFd(this->name, setUtil::setUnion(fd.left, fd.right), this->fds, true, this->fullyFindImps, this->printFDs);
            Relation newRelFromRest(this->name, setUtil::difference(this->atts, fd.right), this->fds, true, this->fullyFindImps, this->printFDs);
            decomp = setUtil::setUnion(newRelFromFd.decompBCNFhelper(), newRelFromRest.decompBCNFhelper());
            break;
        }
    }

    // every FD in the minimal cover that is not already in the decomposition needs to be added in
    for (const FunctionalDependency& fd : minCov) {
        if (!absFunc::ormap_f<Relation>(setUtil::setToVector(decomp), [&fd] (const Relation& rel) {
            return setUtil::isSuperset(rel.atts, setUtil::setUnion(fd.left, fd.right));
        })) {
            decomp.insert(Relation(this->name, setUtil::setUnion(fd.left, fd.right), this->fds, true, this->fullyFindImps, this->printFDs));
        }
    }

    vector<Relation> decompVec = setUtil::setToVector(rel::removeRedundantRelations(decomp));
    vector<string> labels = statUtil::generateNumberLabels(1, decompVec.size());

    int currLabel = 0;
    for (Relation& rel : decompVec) {
        rel.name = this->name + "." + labels.at(currLabel++);
    }

    return setUtil::vectorToSet(decompVec);

}

bool Relation::operator < (const Relation& other) const {
    return tie(name, atts, fds) < tie(other.name, other.atts, other.fds);
}

bool Relation::operator == (const Relation& other) const {
    return tie(name, atts, fds) == tie(other.name, other.atts, other.fds);
}

ostream& operator << (ostream& out, const Relation& rel) {
    
    string output = rel.name + "(";
    for (Attribute att : rel.atts) {
        if (rel.findAllKeys().begin()->count(att)) {
            output += ANSI_GREEN;
        }
        output += att + ANSI_NORMAL + ", ";
    }
    output = output.substr(0, output.size() - 2) + ")";
    out << output;

    if (rel.printFDs) {
        out << ANSI_BLUE;
        for (const FunctionalDependency& fd : rel.fds) {
            out << "\n" << fd;
        }
        out << ANSI_NORMAL;
    }

    return out;

}

ostream& operator << (ostream& out, const set<Relation>& rels) {
    if (rels.empty()) {
        out << ANSI_RED << "\nThere are no relations to display.\n" << ANSI_NORMAL;
    } else {
        for (const Relation& rel : rels) {
            out << "\n" << rel << "\n";
        }
    }
    return out;
}

Relation rel::getByName(const set<Relation>& rels, string name) {
    for (const Relation& rel : rels) {
        if (rel.name == name) {
            return rel;
        }
    }
    throw runtime_error("no relation named " + name);
}

set<Relation> rel::readFromFile(const string& filename, bool preprocess, bool full, bool printFDs) {

    vector<string> data;
    file::inputStrVecFrom(data, filename);

    set<Relation> rels;
    map<string, string> relationToParent;

    string schema;
    set<string> rawFds;
    string parentName;

    for (const string& line : data) {
        if (!line.empty()) {
            
            if (strUtil::contains(line, "(")) {
                // this is the schema line
                schema = line;
            } else if (strUtil::beginsWith(line, "@PARENT")) {
                // this is an inheritance line
                parentName = parse::parseArgumentUntilEnd(line);
            } else {
                // this is an FD line
                rawFds.insert(line);
            }

        } else {

            if (!schema.empty()) {
                rels.insert(Relation(schema, rawFds, preprocess, full, printFDs));
                relationToParent.insert({strUtil::removeAllAfterChar(schema, '('), parentName});
            }
            
            schema.clear();
            rawFds.clear();
            parentName.clear();
        
        }
    }

    if (!schema.empty()) {
        rels.insert(Relation(schema, rawFds, preprocess, full, printFDs));
        relationToParent.insert({strUtil::removeAllAfterChar(schema, '('), parentName});
    }

    set<Relation> relsWithProcessedParents;
    
    for (const Relation& rel : rels) {

        Relation newRel = rel;
        string currParentName = relationToParent.at(newRel.name);

        if (!currParentName.empty()) {
            newRel.inheritFDsFrom(rel::getByName(rels, currParentName), preprocess);
        }

        relsWithProcessedParents.insert(newRel);

    }
    
    return relsWithProcessedParents;

}

set<Relation> rel::removeRedundantRelations(const set<Relation>& rels) {

    auto isRedundant = [&rels] (const Relation& rel) -> bool {
        for (const Relation& r : rels) {
            if (setUtil::isSuperset(r.atts, rel.atts, true)) {
                return true;
            }
        }
        return false;
    };

    vector<Relation> relations = setUtil::setToVector(rels);
    vector<int> indexesToRemove;

    for (int i = 0; i < relations.size(); i++) {
        if (isRedundant(relations.at(i))) {
            indexesToRemove.push_back(i);
        }
    }

    vecUtil::removeByIndexes(relations, indexesToRemove);
    return setUtil::vectorToSet(relations);

}