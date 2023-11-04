#ifndef TEST_DOT_H
#define TEST_DOT_H

#include "../Relation.h"

namespace test {

    void testFDandRelationIO();
    void testReadFromFile();

    void testClosure();
    void testFindAllFDs();
    void testRemoveIrrelevant();
    void testIsLossless();
    void testKeysAndSuperkeys();
    void testFindAllKeysAndSuperkeys();

    void testBCNFdecomp();
    void testMinimalCover();
    void test3NFdecompLJ();
    void test3NFrequireFull();
    void test3NFdecompSyn();
    void test3NFisPossiblyNotInBCNF();

    void testProject();

}

#endif