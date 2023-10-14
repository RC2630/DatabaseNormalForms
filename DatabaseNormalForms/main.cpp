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

void run() {
    
    cout << "\nWelcome to the Database Normal Forms program! Please type " << ANSI_YELLOW << "/help" << ANSI_NORMAL << " to get started.\n";
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
        } else {
            cout << ANSI_RED << "\nInvalid command or wrong number of arguments. Try again!\n" << ANSI_NORMAL;
        }

    }

}

int main() {
    run();
}