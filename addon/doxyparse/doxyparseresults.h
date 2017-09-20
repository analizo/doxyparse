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
    std::map<std::string, bool> modules;
    std::string current_module;
    YAML::Emitter yaml;

    void detectProgrammingLanguage(FileNameListIterator& fnli);
    bool checkLanguage(std::string& filename, std::string extension);
    void printFile(std::string file);
    void printModule(std::string module);
    void listMembers(MemberList *ml);
    void printDefines();
    void lookupSymbol(Definition *d);
    void printDefinition(std::string type, std::string signature, int line);
    void printProtection(std::string protection);
    void functionInformation(MemberDef* md);
    void printNumberOfLines(int lines);
    void printNumberOfArguments(int arguments);
    void printUses();
    void printNumberOfConditionalPaths(MemberDef* md);
    bool ignoreStaticExternalCall(MemberDef *context, MemberDef *md);
    void referenceTo(MemberDef* md);
    int isPartOfCStruct(MemberDef * md);
    std::string functionSignature(MemberDef* md);
    void printReferenceTo(std::string type, std::string signature,
                          std::string defined_in);
    void classInformation(ClassDef* cd);
    void cModule(ClassDef* cd);
    void printInheritance(std::string base_class);
    void printClassInformation(std::string information);
    void listAllMembers(ClassDef* cd);
};
#endif
