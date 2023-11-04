#include "test.h"

#include "../general/setUtility.h"
#include "../general/abstractFunctions.h"
#include "../general/customPrint.h"
#include "../general/ansi_codes.h"

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

void test::testFindAllKeysAndSuperkeys() {

    set<Relation> relationsWithPreprocessing = rel::readFromFile("file/testing/test_find_all_keys.txt");
    set<Relation> relationsWithoutPreprocessing = rel::readFromFile("file/testing/test_find_all_keys.txt", false);
    vector<string> relnames = absFunc::map_f<Relation, string>(setUtil::setToVector(relationsWithPreprocessing), [] (const Relation& r) {
        return r.name;
    });

    auto printAttributeSet = [] (ostream& out, const set<Attribute>& atts) -> ostream& {
        return out << atts;
    };

    auto displayKeys = [printAttributeSet] (const set<set<Attribute>>& keys, string keyOrSuperkey) -> void {
        if (keys.size() > 20) {
            cout << "(too many " << keyOrSuperkey << "s to display: there are " << keys.size() << " " << keyOrSuperkey << "s)";
        } else {
            cprint::print<set<Attribute>>(cout, keys, printAttributeSet);
        }
    };

    for (string name : relnames) {
        cout << "\nkeys for " << name << " (with preprocessing):\n";
        displayKeys(rel::getByName(relationsWithPreprocessing, name).findAllKeys(), "key");
        cout << "\n\nkeys for " << name << " (without preprocessing):\n";
        displayKeys(rel::getByName(relationsWithoutPreprocessing, name).findAllKeys(), "key");
        cout << "\n\nsuperkeys for " << name << " (with preprocessing):\n";
        displayKeys(rel::getByName(relationsWithPreprocessing, name).findAllSuperkeys(), "superkey");
        cout << "\n\nsuperkeys for " << name << " (without preprocessing):\n";
        displayKeys(rel::getByName(relationsWithoutPreprocessing, name).findAllSuperkeys(), "superkey");
        cout << "\n";
    }

}

void test::testBCNFdecomp() {
    set<Relation> rels = rel::readFromFile("file/testing/test_bcnf_decomp.txt");
    for (const Relation& rel : rels) {
        cout << ANSI_RED << "\nRelation:\n\n" << ANSI_NORMAL << rel
             << ANSI_RED << "\n\nDecomposition " << ANSI_YELLOW << "(" << (rel.isInBCNF() ? "" : "NOT ") << "in BCNF)"
             << ANSI_RED << ":\n" << ANSI_NORMAL;
        for (const Relation& decompRel : rel.decompBCNF()) {
            cout << "\n" << decompRel << "\n";
        }
        cout << ANSI_YELLOW << "\n" << string(100, '-') << "\n" << ANSI_NORMAL;
    }
}

void test::testMinimalCover() {
    set<Relation> rels = rel::readFromFile("file/testing/test_minimal_cover.txt");
    for (const Relation& rel : rels) {
        cout << ANSI_RED << "\nRelation:\n\n" << ANSI_NORMAL << rel
             << ANSI_RED << "\n\nMinimal Cover:\n" << ANSI_NORMAL << fd::minimalCover(rel.fds)
             << "\n" << ANSI_YELLOW << string(100, '-') << "\n" << ANSI_NORMAL;
    }
}

void test::test3NFdecompLJ() {
    set<Relation> rels = rel::readFromFile("file/testing/test_3nf_decomp.txt");
    for (const Relation& rel : rels) {
        cout << ANSI_RED << "\nRelation:\n\n" << ANSI_NORMAL << rel
             << ANSI_RED << "\n\nDecomposition " << ANSI_YELLOW << "(" << (rel.isIn3NF() ? "" : "NOT ") << "in 3NF)"
             << ANSI_RED << ":\n" << ANSI_NORMAL;
        for (const Relation& decompRel : rel.decomp3NFlosslessJoin()) {
            cout << "\n" << decompRel << "\n";
        }
        cout << ANSI_YELLOW << "\n" << string(100, '-') << "\n" << ANSI_NORMAL;
    }
}

void test::test3NFrequireFull() {

    set<Relation> relsReg = rel::readFromFile("file/testing/test_3nf_full.txt", true, false, false);
    Relation s108Reg = rel::getByName(relsReg, "S108");
    cout << ANSI_GREEN << "\n(NOT FULL)\n" << ANSI_RED << "\nRelation:\n\n" << ANSI_NORMAL << s108Reg
         << ANSI_RED << "\n\nDecomposition " << ANSI_YELLOW << "(" << (s108Reg.isIn3NF() ? "" : "NOT ") << "in 3NF)"
         << ANSI_RED << ":\n" << ANSI_NORMAL;
    for (const Relation& decompRel : s108Reg.decomp3NFlosslessJoin()) {
        cout << "\n" << decompRel << "\n";
    }
    cout << ANSI_YELLOW << "\n" << string(100, '-') << "\n" << ANSI_NORMAL;

    set<Relation> relsFull = rel::readFromFile("file/testing/test_3nf_full.txt", true, true, false);
    Relation s108Full = rel::getByName(relsFull, "S108");
    cout << ANSI_GREEN << "\n(FULL)\n" << ANSI_RED << "\nRelation:\n\n" << ANSI_NORMAL << s108Full
         << ANSI_RED << "\n\nDecomposition " << ANSI_YELLOW << "(" << (s108Full.isIn3NF() ? "" : "NOT ") << "in 3NF)"
         << ANSI_RED << ":\n" << ANSI_NORMAL;
    for (const Relation& decompRel : s108Full.decomp3NFlosslessJoin()) {
        cout << "\n" << decompRel << "\n";
    }

}

void test::test3NFdecompSyn() {
    set<Relation> rels = rel::readFromFile("file/testing/test_3nf_decomp.txt");
    for (const Relation& rel : rels) {
        cout << ANSI_RED << "\nRelation:\n\n" << ANSI_NORMAL << rel
             << ANSI_RED << "\n\nDecomposition " << ANSI_YELLOW << "(" << (rel.isIn3NF() ? "" : "NOT ") << "in 3NF)"
             << ANSI_RED << ":\n" << ANSI_NORMAL;
        for (const Relation& decompRel : rel.decomp3NFsynthesis()) {
            cout << "\n" << decompRel << "\n";
        }
        cout << ANSI_YELLOW << "\n" << string(100, '-') << "\n" << ANSI_NORMAL;
    }
}

void test::test3NFisPossiblyNotInBCNF() {
    set<Relation> rels = rel::readFromFile("file/testing/test_3nf_possibly_not_in_bcnf.txt");
    for (const Relation& rel : rels) {
        cout << ANSI_RED << "\nRelation:\n\n" << ANSI_NORMAL << rel
             << ANSI_RED << "\n\nPossibly not in BCNF? " << boolalpha << ANSI_NORMAL;
        try {
            cout << rel.possiblyNotInBCNFgiven3NF();
        } catch (const invalid_argument& e) {
            cout << "exception: relation not in 3NF";
        }
        cout << ANSI_YELLOW << "\n\n" << string(100, '-') << "\n" << ANSI_NORMAL;
    }
}

void test::testProject() {
    set<Relation> rels = rel::readFromFile("file/project/project_relations.txt");
    for (const Relation& rel : rels) {
        if (rel.name != "Everything") {
            cout << ANSI_RED << "\nRelation:\n\n" << ANSI_NORMAL << rel
                 << ANSI_RED << "\n\nDecomposition " << ANSI_YELLOW << "(" << (rel.isInBCNF() ? "" : "NOT ") << "in BCNF)"
                 << ANSI_RED << ":\n" << ANSI_NORMAL << rel.decompBCNF()
                 << ANSI_YELLOW << "\n" << string(100, '-') << "\n" << ANSI_NORMAL;
        }
    }
}