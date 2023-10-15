#include "Relation.h"

#include "general/stringUtility.h"
#include "general/parseArguments.h"
#include "general/ansi_codes.h"
#include "general/file.h"
#include "general/mapUtility.h"

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
    // TODO
    return {{"Dog", "Cat", "Person"}};
}

void Relation::inheritFDsFrom(const Relation& parent, bool removeIrrelevant) {
    this->fds = parent.fds;
    if (removeIrrelevant) {
        this->fds = fd::removeIrrelevantFDs(this->fds, this->atts);
    }
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