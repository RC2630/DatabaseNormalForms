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

}

#endif