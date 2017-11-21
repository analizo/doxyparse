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
    const std::string TYPE = "type";
    const std::string ABSTRACT_CLASS = "abstract class";
    const std::string VARIABLE = "variable";
    const std::string INHERITS = "inherits";
    const std::string INFORMATIONS = "informations";
    const std::string DEFINES = "defines";
    const std::string LINE = "line";
    const std::string PROTECTION = "protection";
    const std::string PUBLIC = "public";
    const std::string LINES_OF_CODE = "lines_of_code";
    const std::string PARAMETERS = "parameters";
    const std::string USES = "uses";
    const std::string DEFINED_IN = "defined_in";
    const std::string CONDITIONAL_PATHS = "conditional_paths";

    bool is_c_code;
    std::map<std::string, bool> modules;
    std::string current_module;
    YAML::Emitter *yaml;
		void loadFileMembersIntoYaml(MemberList *member_list, FileDef *file_definition, ClassSDict *classes);
    void verifyEmptyMemberListOrClasses(MemberList *member_list,
                                        ClassSDict *classes,
                                        enum YAML::EMITTER_MANIP value);
    FileNameListIterator getFiles();
    void detectIsCCode(FileNameListIterator& fnli);
    bool checkLanguage(std::string& filename, std::string extension);
    void printFile(std::string file);
    void printModule(std::string module);
    void listMembers(MemberList *ml);
    void listMembers2(MemberList *ml);
    void printDefines();
    void lookupSymbol(Definition *d);
    void printDefinition(std::string type, std::string signature,
                         int line, Definition *d);
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
    void addValue(std::string key, std::string value);
    void addValue(std::string key, int value);
    void addValue(std::string key);
    void addValue(std::string key, enum YAML::EMITTER_MANIP value);
    void addKeyYaml(std::string key);

};
#endif
