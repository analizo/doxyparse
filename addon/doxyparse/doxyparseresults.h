#ifndef DOXYPARSERESULTS_H
#define DOXYPARSERESULTS_H

#include "doxygen.h"
#include "arguments.h"
#include "classlist.h"
#include "filedef.h"
#include "filename.h"
#include <string>
#include <yaml-cpp/yaml.h>

class DoxyparseResults
{
  public:
    DoxyparseResults();
   ~DoxyparseResults();

    void listSymbols();

  private:
    bool is_c_code;

    YAML::Emitter yaml;

    void detectProgrammingLanguage(FileNameListIterator &fnli);
    bool checkLanguage(std::string &filename, std::string extension);
    void printFile(FileDef *fd);
    void listMembers(MemberList *ml);
    void lookupSymbol(Definition *d);
    void printDefinition(MemberDef *md);
    void printType(MemberDef *md);
    void printSignature(MemberDef *md);
    void printArgumentList(MemberDef *md);
    void printDefinitionLine(MemberDef *md);
    void printProtection(MemberDef *md);
    void printFunctionInformation(MemberDef *md);
    void printNumberOfLines(MemberDef *md);
    void printNumberOfArguments(MemberDef *md);
    void printNumberOfConditionalPaths(MemberDef *md);
    void printReferencesMembers(MemberDef *md);
    bool ignoreStaticExternalCall(MemberDef *context, MemberDef *md);
    void printReferenceTo(MemberDef *md);
    int isPartOfCStruct(MemberDef *md);
    void printCStructMember(MemberDef *md);
    void printWhereItWasDefined(MemberDef *md);
    void printClass(ClassDef *cd);
    void printCModule(ClassDef *cd);
    void printClassInformation(ClassDef *cd);
    void printInheritance(ClassDef *cd);
    void listAllMembers(ClassDef *cd);
};
#endif
