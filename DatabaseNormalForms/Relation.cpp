#include "Relation.h"

#include "general/stringUtility.h"
#include "general/parseArguments.h"
#include "general/ansi_codes.h"
#include "general/file.h"
#include "general/mapUtility.h"
#include "general/setUtility.h"
#include "general/abstractFunctions.h"

#include <queue>

Relation::Relation(const string& name, const set<Attribute>& atts, const set<FunctionalDependency>& fds, bool preprocess)
: name(name), atts(atts) {
    if (preprocess) {
        this->fds = fd::findAllFunctionalDependencies(fds);
        this->fds = fd::removeIrrelevantFDs(this->fds, this->atts);
    } else {
        this->fds = fds;
    }
}

Relation::Relation(const string& schema, const set<string>& rawFds, bool preprocess) {
    
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
        this->fds = fd::findAllFunctionalDependencies(this->fds);
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

set<Relation> rel::readFromFile(const string& filename, bool preprocess) {

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
                rels.insert(Relation(schema, rawFds, preprocess));
                relationToParent.insert({strUtil::removeAllAfterChar(schema, '('), parentName});
            }
            
            schema.clear();
            rawFds.clear();
            parentName.clear();
        
        }
    }

    if (!schema.empty()) {
        rels.insert(Relation(schema, rawFds, preprocess));
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