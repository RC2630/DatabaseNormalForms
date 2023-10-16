#include "test.h"

#include "../general/setUtility.h"
#include "../general/abstractFunctions.h"
#include "../general/customPrint.h"

using setUtil::operator<<;
using cprint::operator<<;

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

void test::testIsLossless() {
    set<Relation> rels = rel::readFromFile("file/testing/test_is_lossless.txt");
    Relation full = rel::getByName(rels, "SuperRentInfo");
    set<Relation> lossy = {rel::getByName(rels, "SI1"), rel::getByName(rels, "SI2")};
    set<Relation> lossless = {rel::getByName(rels, "SI3"), rel::getByName(rels, "SI4")};
    cout << boolalpha
         << "\nis 'lossy' a lossless decomposition for 'full'? " << full.isDecompLossless(lossy)
         << "\nis 'lossless' a lossless decomposition for 'full'? " << full.isDecompLossless(lossless) << "\n";
}

void test::testKeysAndSuperkeys() {
    cout << boolalpha;
    Relation supplierPart = *rel::readFromFile("file/testing/test_keys_and_superkeys.txt").begin();
    set<set<Attribute>> potentialKeys = {
        {"sname"},
        {"sname", "p#"},
        {"sname", "p#", "pname"}
    };
    for (const set<Attribute>& potentialKey : potentialKeys) {
        cout << "\n" << potentialKey << " is a superkey? " << supplierPart.isSuperkey(potentialKey)
             << "\n" << potentialKey << " is a key? " << supplierPart.isKey(potentialKey) << "\n";
    }
}

void test::testFindAllKeys() {

    set<Relation> relationsWithPreprocessing = rel::readFromFile("file/testing/test_find_all_keys.txt");
    set<Relation> relationsWithoutPreprocessing = rel::readFromFile("file/testing/test_find_all_keys.txt", false);
    vector<string> relnames = absFunc::map_f<Relation, string>(setUtil::setToVector(relationsWithPreprocessing), [] (const Relation& r) {
        return r.name;
    });

    auto printAttributeSet = [] (
        ostream& out, const set<Attribute>& atts
    ) -> ostream& {
        return out << atts;
    };

    for (string name : relnames) {
        cout << "\nkeys for " << name << " (with preprocessing):\n";
        cprint::print<set<Attribute>>(cout, rel::getByName(relationsWithPreprocessing, name).findAllKeys(), printAttributeSet);
        cout << "\n\nkeys for " << name << " (without preprocessing):\n";
        cprint::print<set<Attribute>>(cout, rel::getByName(relationsWithoutPreprocessing, name).findAllKeys(), printAttributeSet);
        cout << "\n";
    }

}