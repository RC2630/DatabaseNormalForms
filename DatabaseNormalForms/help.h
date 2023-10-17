#ifndef HELP_DOT_H
#define HELP_DOT_H

#include "general/ansi_codes.h"

#include <string>

using namespace std;

namespace help {

    const string HELP =

        "/help = display this help document\n"
        "/help file = display the help document for storing relations in the file that this program reads from\n"
        "/help input = display the help document for inputting attributes, functional dependencies, and relations into this program\n"
        "/quit = exit program\n\n"

        "/list = display all relations\n"
        "/view <name> = display information about the relation with given name\n\n"
        
        "/closure = take closure of a set of attributes with respect to a set of functional dependencies\n"
        "/findall = find all implicit FD's, transform them into new FD's with single-attribute right sides, and remove trivial FD's\n"
        "/islossless = check if a decomposition (set of relations) for a relation is a lossless one or not\n"
        "/issuperkey = check if a set of attributes form a superkey for a relation\n"
        "/iskey = check if a set of attributes form a (minimal) key for a relation\n"
        "/findkeys = find all (minimal) keys of a relation\n"
        "/findsuperkeys = find all superkeys of a relation\n\n"
        
        "/violbcnf = check if a (non-trivial and single-attribute-right-side) functional dependency violates BCNF";

    const string FILE_HELP =

        "All relations should be placed into \"file/relations.txt\" prior to starting this program.\n\n"

        "Every relation begins with a schema line, which contains its name and attributes.\n"
        "Following that, you can either specify an inheritance line with the \"@PARENT\" keyword followed by the name of the parent relation,\n"
        "or you can specify the functional dependencies of the relation directly (1 function dependency per line).\n\n"

        "If an inheritance line is used, the relation's functional dependencies consist of\n"
        "all the relevant functional dependencies of the parent relation.\n"
        "Please use the name of the original/full relation as the parent,\n"
        "rather than intermediate relations between the current relation and the full relation,\n"
        "since the program can only handle 1 layer of inheritance\n"
        "(because inheriting from the original relation is functionally the same as inheriting from an intermediate relation).\n\n"

        "You can place as many relations as you want into the file, as long as they do not share a name.\n"
        "Please leave a blank line between relations.\n\n"

        "See \"file/example_relations.txt\" for some examples of relations.";

    const string INPUT_HELP =

        "When prompted for an attribute, enter the name of the attribute.\n\n"

        "When prompted for a set of attributes, enter all the names of the attributes on 1 line.\n"
        "Separate attribute names with a comma and a space (ex. \"name, grade, age\").\n"
        "Alternatively, you can enter \"@\" followed by a relation name (ex. \"@Animal\"),\n"
        "and all the attributes in that relation will be used.\n\n"

        "When prompted for a functional dependency, enter it using the following syntax:\n"
        "<left_1>, <left_2>, ..., <left_n> -> <right_1>, <right_2>, ..., <right_n>\n"
        "(ex. \"name, age -> grade, teacher\")\n"
        "Note that attribute names are separated by a comma and a space, and the middle arrow (\"->\") has a space before and after it.\n\n"

        "When prompted for a set of functional dependencies, enter each functional dependency on its own line.\n"
        "After all the functional dependencies have been entered, type a single dot (\".\") on its own line to finish the input.\n"
        "Alternatively, you can enter \"@\" followed by a relation name (ex. \"@Animal\"),\n"
        "and all the functional dependencies of that relation will be used.\n"
        "Note that due to preprocessing of functional dependencies, the set of functional dependencies obtained from a relation through\n"
        "this method may not be the same as the set of functional dependencies specified for that relation in \"file/relations.txt\".\n\n"

        "Preprocessing includes:\n"
        "1. inheriting parent functional dependencies\n"
        "2. finding implicit functional dependencies\n"
        "3. splitting single functional dependencies with multiple attributes on the right side into\n"
        "   multiple functional dependencies with a single attribute on the right side\n"
        "4. removing trivial functional dependencies (where the attribute from the right side appears on the left side)\n"
        "5. removing irrelevant functional dependencies (where an attribute in the functional dependency does not appear in the relation)\n\n"

        "When prompted for a relation, enter only the name of the relation.\n"
        "All relations must be placed into \"file/relations.txt\" before the start of the program.\n"
        "Please type " + ANSI_YELLOW + "/help file" + ANSI_BLUE + " for details.\n\n"

        "When prompted for a set of relations, enter all the names of the relations on 1 line.\n"
        "Separate relation names with a comma and a space (ex. \"Animal, Person, Environment\").";

}

#endif