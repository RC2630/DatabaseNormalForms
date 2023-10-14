#include "Relation.h"
#include "test/test.h"

#include "general/ansi_codes.h"
#include "general/parseArguments.h"

const string HELP =

    "/help = display this help document\n"
    "/quit = exit program\n\n"

    "/list = display all relations\n"
    "/view <name> = display information about the relation with given name";

set<Relation> rels = rel::readFromFile("file/relations.txt");

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
        cout << ANSI_RED << "\nSorry, but there is no relation named \"" << relname << "\".\n" << ANSI_NORMAL;
    }
}

void run() {
    
    cout << "\nWelcome to the Database Normal Forms program! Please type " << ANSI_YELLOW << "/help" << ANSI_NORMAL << " to get started.\n";
    string command;

    while (true) {
        
        cout << ANSI_YELLOW << "\n>>> " << ANSI_GREEN;
        getline(cin >> ws, command);
        cout << ANSI_NORMAL;

        if (parse::commandIs(command, "/help")) {
            cout << ANSI_BLUE << "\n" << HELP << "\n" << ANSI_NORMAL;
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