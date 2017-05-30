#ifndef DOXYPARSE_H
#define DOXYPARSE_H

#include "doxyparseinterface.h"
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include "doxygen.h"
#include "outputgen.h"
#include "parserintf.h"
#include "classlist.h"
#include "config.h"
#include "filedef.h"
#include "util.h"
#include "filename.h"
#include "arguments.h"
#include "memberlist.h"
#include "types.h"
#include <string>
#include <cstdlib>

class Doxyparse
{
  public:
    std::ostringstream tmpdir;
    bool is_c_code;

    Doxyparse();
   ~Doxyparse();

    void doxygenParseInput();
    void initializeDoxygen();
    void setConfiguration();
    void setInput(int argc, char **argv);
    void parseReferences();
    void removeOutputDir();
    void removeTemporaryDir();

    void findXRefSymbols(FileDef *fd);
    bool ignoreStaticExternalCall(MemberDef *context, MemberDef *md);
    void printArgumentList(MemberDef *md);
    void printType(MemberDef *md);
    void printSignature(MemberDef *md);
    void printWhereItWasDefined(MemberDef *md);
    void printCStructMember(MemberDef *md);
    int isPartOfCStruct(MemberDef *md);
    void printReferenceTo(MemberDef *md);
    void printReferencesMembers(MemberDef *md);
    void printDefinitionLine(MemberDef *md);
    void printDefinition(MemberDef *md);
    void printProtection(MemberDef *md);
    void printNumberOfLines(MemberDef *md);
    void printNumberOfArguments(MemberDef *md);
    void printNumberOfConditionalPaths(MemberDef *md);
    void printFunctionInformation(MemberDef *md);
    void lookupSymbol(Definition *d);
    void listMembers(MemberList *ml);
    void printInheritance(ClassDef *cd);
    void printCModule(ClassDef *cd);
    void listAllMembers(ClassDef *cd);
    void printClassInformation(ClassDef *cd);
    void printClass(ClassDef *cd);
    void printFile(FileDef *fd);
    bool checkLanguage(std::string &filename, std::string extension);
    void detectProgrammingLanguage(FileNameListIterator &fnli);
    void listSymbols();
};

#endif
