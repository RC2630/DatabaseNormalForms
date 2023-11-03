#include "Relation.h"
#include "help.h"

#include "test/test.h"

#include "general/ansi_codes.h"
#include "general/parseArguments.h"
#include "general/setUtility.h"

using setUtil::operator<<;

set<Relation> rels = rel::readFromFile("file/relations.txt");

string noRelationErrorMessage(string relname) {
    return ANSI_RED + "\nSorry, but there is no relation named \"" + relname + "\".\n" + ANSI_NORMAL;
}

set<Attribute> inputAttributeSet() {

    string attList;
    cout << ANSI_GREEN;
    getline(cin >> ws, attList);
    cout << ANSI_NORMAL;

    if (strUtil::beginsWith(attList, "@")) {
        string relName = attList.substr(1);
        try {
            return rel::getByName(rels, relName).atts;
        } catch (const runtime_error& e) {
            throw runtime_error(relName);
        }
    }

    vector<string> atts = parse::parseAllArguments(attList, true, ',');
    for (string& s : atts) {
        if (strUtil::beginsWith(s, " ")) {
            s = s.substr(1);
        }
    }

    return set<Attribute>(atts.begin(), atts.end());

}

FunctionalDependency inputFD() {

    string fd;
    cout << ANSI_GREEN;
    getline(cin >> ws, fd);
    cout << ANSI_NORMAL;

    return FunctionalDependency(fd);

}

set<FunctionalDependency> inputFDset() {

    set<FunctionalDependency> fdSet;

    while (true) {
        try {
            fdSet.insert(inputFD());
        } catch (const invalid_argument& e) { // end of input (most likely a dot)
            return fdSet;
        } catch (const runtime_error& e) { // relation pointer (@Relation)
            string relName = string(e.what()).substr(1);
            try {
                return rel::getByName(rels, relName).fds;
            } catch (const runtime_error& e) {
                throw runtime_error(relName);
            }
        }
    }

    throw runtime_error("this line should never be reached");

}

Relation inputRelation() {
    
    string relname;
    cout << ANSI_GREEN;
    getline(cin >> ws, relname);
    cout << ANSI_NORMAL;

    try {
        return rel::getByName(rels, relname);
    } catch (const runtime_error& e) {
        throw runtime_error(relname); // get back relname with e.what()
    }

}

set<Relation> inputRelationSet() {
    
    // abusing this helper here since a set of relation names is functionally identical to a set of attribute names
    set<string> relnames = inputAttributeSet();
    set<Relation> relSet;

    for (string name : relnames) {
        try {
            relSet.insert(rel::getByName(rels, name));
        } catch (const runtime_error& e) {
            throw runtime_error(name); // get back name with e.what()
        }
    }

    return relSet;

}

void list() {
    
    if (rels.empty()) {
        cout << ANSI_RED << "\nSorry, but there are no relations in the program right now.\n" << ANSI_NORMAL;
        return;
    }

    for (const Relation& r : rels) {
        cout << "\n" << r << "\n";
    }

}

void view(const string& relname) {
    try {
        Relation r = rel::getByName(rels, relname);
        cout << "\n" << r << "\n";
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(relname);
    }
}

void closure() {
    cout << "\nEnter a set of attributes: ";
    set<Attribute> atts = inputAttributeSet();
    cout << "\nEnter a set of functional dependencies:\n";
    set<FunctionalDependency> fds = inputFDset();
    cout << "\nThe closure is: " << ANSI_BLUE << fd::closure(atts, fds) << ANSI_NORMAL << "\n";
}

void findall() {
    cout << "\nEnter a set of functional dependencies:\n";
    set<FunctionalDependency> fds = inputFDset();
    cout << "\nHere are the resulting functional dependencies:" << ANSI_BLUE << fd::findAllFunctionalDependencies(fds) << ANSI_NORMAL;
}

void isLossless() {
    try {
        cout << "\nEnter the name of the full relation: ";
        Relation full = inputRelation();
        cout << "Enter the names of the 2 relations that comprise the decomposition of the full relation: ";
        set<Relation> decomp = inputRelationSet();
        bool isLosslessResult = full.isDecompLossless(decomp);
        cout << ANSI_BLUE << "\nThe given decomposition is " << (isLosslessResult ? "" : "NOT ")
             << "a lossless decomposition of the full relation.\n" << ANSI_NORMAL;
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    } catch (const invalid_argument& e) {
        cout << ANSI_RED << "\nYou specified the wrong number of relations in the given decomposition (must be 2).\n" << ANSI_NORMAL;
    }
}

void isSuperkey() {
    try {
        cout << "\nEnter the name of the relation: ";
        Relation rel = inputRelation();
        cout << "Enter the attributes that you would like to check if they form a superkey: ";
        set<Attribute> atts = inputAttributeSet();
        cout << ANSI_BLUE << "\nThe given attributes " << (rel.isSuperkey(atts) ? "" : "do NOT ")
             << "form a superkey of the relation.\n" << ANSI_NORMAL;
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    }
}

void isKey() {
    try {
        cout << "\nEnter the name of the relation: ";
        Relation rel = inputRelation();
        cout << "Enter the attributes that you would like to check if they form a key: ";
        set<Attribute> atts = inputAttributeSet();
        cout << ANSI_BLUE << "\nThe given attributes " << (rel.isKey(atts) ? "" : "do NOT ") << "form a key of the relation.\n" << ANSI_NORMAL;
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    }
}

void findAllKeys() {
    try {
        cout << "\nEnter the name of the relation: ";
        Relation rel = inputRelation();
        cout << "\nHere are all the (minimal) candidate keys for your relation:\n" << ANSI_BLUE;
        for (const set<Attribute>& key : rel.findAllKeys()) {
            cout << key << "\n";
        }
        cout << ANSI_NORMAL;
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    }
}

void findAllSuperkeys() {
    try {
        cout << "\nEnter the name of the relation: ";
        Relation rel = inputRelation();
        cout << "\nHere are all the superkeys for your relation:\n" << ANSI_BLUE;
        for (const set<Attribute>& superkey : rel.findAllSuperkeys()) {
            cout << superkey << "\n";
        }
        cout << ANSI_NORMAL;
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    }
}

void removeRedundant() {
    try {
        cout << "\nEnter the set of relations to check: ";
        set<Relation> relsToCheck = inputRelationSet();
        cout << "\nHere are the non-redundant relations that should be kept:\n" << rel::removeRedundantRelations(relsToCheck);
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    }
}

void violatesBCNF() {
    try {
        cout << "\nEnter the name of the relation: ";
        Relation rel = inputRelation();
        cout << "Enter the functional dependency to check: ";
        FunctionalDependency fd = inputFD();
        cout << ANSI_BLUE << "\nThe given functional dependency "
             << (rel.violatesBCNF(fd) ? "violates" : "does NOT violate") << " BCNF.\n" << ANSI_NORMAL;
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    }
}

void checkBCNF() {
    try {
        cout << "\nEnter the name of the relation: ";
        Relation rel = inputRelation();
        cout << ANSI_BLUE << "\nYour relation is currently " << (rel.isInBCNF() ? "" : "NOT ") << "in BCNF.\n" << ANSI_NORMAL;
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    }
}

void decomposeBCNF() {
    try {
        cout << "\nEnter the name of the relation: ";
        Relation rel = inputRelation();
        cout << "\nHere are the relations in the BCNF decomposition of your relation:\n";
        for (const Relation& r : rel.decompBCNF()) {
            cout << "\n" << r << "\n";
        }
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    }
}

void minimalCover() {
    cout << "\nEnter a set of functional dependencies:\n";
    set<FunctionalDependency> fds = inputFDset();
    cout << "\nHere is the minimal cover for your set of functional dependencies:"
         << ANSI_BLUE << fd::minimalCover(fds) << ANSI_NORMAL;
}

void violates3NF() {
    try {
        cout << "\nEnter the name of the relation: ";
        Relation rel = inputRelation();
        cout << "Enter the functional dependency to check: ";
        FunctionalDependency fd = inputFD();
        cout << ANSI_BLUE << "\nThe given functional dependency "
            << (rel.violates3NF(fd) ? "violates" : "does NOT violate") << " 3NF.\n" << ANSI_NORMAL;
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    }
}

void check3NF() {
    try {
        cout << "\nEnter the name of the relation: ";
        Relation rel = inputRelation();
        cout << ANSI_BLUE << "\nYour relation is currently " << (rel.isIn3NF() ? "" : "NOT ") << "in 3NF.\n" << ANSI_NORMAL;
    } catch (const runtime_error& e) {
        cout << noRelationErrorMessage(e.what());
    }
}

void run() {
    
    cout << ANSI_NORMAL << "\nWelcome to the Database Normal Forms program! Please type "
         << ANSI_YELLOW << "/help" << ANSI_NORMAL << " to get started.\n";
    string command;

    while (true) {
        
        cout << ANSI_YELLOW << "\n>>> " << ANSI_GREEN;
        getline(cin >> ws, command);
        cout << ANSI_NORMAL;

        if (parse::commandIs(command, "/help") && parse::numArguments(command) == 0) {
            cout << ANSI_BLUE << "\n" << help::HELP << "\n" << ANSI_NORMAL;
        } else if (parse::commandIs(command, "/help") && parse::numArguments(command) == 1 && parse::parseArgument(command) == "file") {
            cout << ANSI_BLUE << "\n" << help::FILE_HELP << "\n" << ANSI_NORMAL;
        } else if (parse::commandIs(command, "/help") && parse::numArguments(command) == 1 && parse::parseArgument(command) == "input") {
            cout << ANSI_BLUE << "\n" << help::INPUT_HELP << "\n" << ANSI_NORMAL;
        } else if (parse::commandIs(command, "/quit")) {
            cout << ANSI_BLUE << "\nThank you for using this program. Bye!\n" << ANSI_NORMAL;
            break;
        } else if (parse::commandIs(command, "/list")) {
            list();
        } else if (parse::commandIs(command, "/view") && parse::numArguments(command) == 1) {
            view(parse::parseArgument(command));
        } else if (parse::commandIs(command, "/closure")) {
            closure();
        } else if (parse::commandIs(command, "/findall")) {
            findall();
        } else if (parse::commandIs(command, "/islossless")) {
            isLossless();
        } else if (parse::commandIs(command, "/issuperkey")) {
            isSuperkey();
        } else if (parse::commandIs(command, "/iskey")) {
            isKey();
        } else if (parse::commandIs(command, "/findkeys")) {
            findAllKeys();
        } else if (parse::commandIs(command, "/findsuperkeys")) {
            findAllSuperkeys();
        } else if (parse::commandIs(command, "/remredundant")) {
            removeRedundant();
        } else if (parse::commandIs(command, "/violbcnf")) {
            violatesBCNF();
        } else if (parse::commandIs(command, "/checkbcnf")) {
            checkBCNF();
        } else if (parse::commandIs(command, "/bcnf")) {
            decomposeBCNF();
        } else if (parse::commandIs(command, "/mincov")) {
            minimalCover();
        } else if (parse::commandIs(command, "/viol3nf")) {
            violates3NF();
        } else if (parse::commandIs(command, "/check3nf")) {
            check3NF();
        } else {
            cout << ANSI_RED << "\nInvalid command or wrong number of arguments. Try again!\n" << ANSI_NORMAL;
        }

    }

}

void tryRun() {
    try {
        run();
    } catch (...) {
        cout << ANSI_RED << "\nOops, an unexpected error occurred. Sorry, but we cannot recover the program from here.\n" << ANSI_NORMAL;
    }
}

int main() {
    tryRun();
}