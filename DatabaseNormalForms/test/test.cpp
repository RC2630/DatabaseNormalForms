#include "test.h"

#include "../general/setUtility.h"

using setUtil::operator<<;

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

void test::testClosure() {
    
    set<FunctionalDependency> fds = {
        FunctionalDependency("sname -> city"),
        FunctionalDependency("city -> status"),
        FunctionalDependency("p# -> pname"),
        FunctionalDependency("sname, p# -> qty")
    };

    set<set<Attribute>> atts = {
        {"sname", "p#"},
        {"sname"},
        {"p#"}
    };

    for (const set<Attribute>& attList : atts) {
        cout << "\n" << attList << "+ = " << fd::closure(attList, fds);
    }
    cout << "\n";

}

void test::testFindAllFDs() {

    set<FunctionalDependency> orig = {
        //FunctionalDependency("A, B -> C"),
        //FunctionalDependency("C -> D"),
        FunctionalDependency("sname -> city"),
        FunctionalDependency("city -> status"),
        FunctionalDependency("p# -> pname"),
        FunctionalDependency("sname, p# -> qty"),
        //FunctionalDependency("A -> A")
    };

    cout << "\noriginal FD's:\n";
    for (const FunctionalDependency& fd : orig) {
        cout << fd << "\n";
    }

    cout << "\nnew FD's:\n";
    for (const FunctionalDependency& fd : fd::findAllFunctionalDependencies(orig)) {
        cout << fd << "\n";
    }

}

void test::testRemoveIrrelevant() {
    Relation r = *rel::readFromFile("file/testing/test_remove_irrelevant.txt", false).begin();
    cout << "\noriginal FD's:\n" << r.fds
         << "\nsimplified FD's:\n" << (r.fds = fd::findAllFunctionalDependencies(r.fds))
         << "\nrelevant FD's:\n" << (r.fds = fd::removeIrrelevantFDs(r.fds, r.atts)) << "\n";
}