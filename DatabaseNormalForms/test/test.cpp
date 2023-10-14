#include "test.h"

void test::testFDandRelationIO() {

    string rawFd = "Person, Dog, Tank Destroyer -> Cat, Person, Family Member, Goblin";
    set<string> rawFds = {rawFd, "Tank, " + rawFd, rawFd + ", Tank"};

    FunctionalDependency fd(rawFd);
    cout << "\n" << fd << "\n";

    Relation rel("Rel(Person, Dog, Tank Destroyer, Cat, Person, Family Member, Goblin, Tank)", rawFds);
    cout << "\n" << rel << "\n";

}

void test::testReadFromFile() {

    set<Relation> rels = rel::readFromFile("file/relations.txt");
    for (const Relation& r : rels) {
        cout << "\n" << r << "\n";
    }

}