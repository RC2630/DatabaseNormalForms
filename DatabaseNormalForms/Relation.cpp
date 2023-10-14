#include "Relation.h"

#include "general/stringUtility.h"
#include "general/parseArguments.h"
#include "general/ansi_codes.h"
#include "general/file.h"

Relation::Relation(const string& name, const set<Attribute>& atts, const set<FunctionalDependency>& fds)
: name(name), atts(atts) {
    this->fds = fd::findAllFunctionalDependencies(fds);
}

Relation::Relation(const string& schema, const set<string>& rawFds) {
    
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
    this->fds = fd::findAllFunctionalDependencies(this->fds);

}

set<set<Attribute>> Relation::findAllKeys() const {
    // TODO
    return {{"Dog", "Cat", "Person"}};
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

bool Relation::operator < (const Relation& other) const {
    return tie(name, atts, fds) < tie(other.name, other.atts, other.fds);
}

bool Relation::operator == (const Relation& other) const {
    return tie(name, atts, fds) == tie(other.name, other.atts, other.fds);
}

Relation rel::getByName(const set<Relation>& rels, string name) {
    for (const Relation& rel : rels) {
        if (rel.name == name) {
            return rel;
        }
    }
    throw runtime_error("no relation named " + name);
}

set<Relation> rel::readFromFile(const string& filename) {

    vector<string> data;
    file::inputStrVecFrom(data, filename);

    set<Relation> rels;
    string schema;
    set<string> rawFds;

    for (const string& line : data) {
        if (!line.empty()) {
            
            if (strUtil::contains(line, "(")) {
                // this is the schema line
                schema = line;
            } else {
                // this is an FD line
                rawFds.insert(line);
            }

        } else {

            if (!schema.empty()) {
                rels.insert(Relation(schema, rawFds));
            }
            
            schema.clear();
            rawFds.clear();
        
        }
    }

    if (!schema.empty()) {
        rels.insert(Relation(schema, rawFds));
    }

    return rels;

}