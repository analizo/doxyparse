#include "doxyparseresults.h"
#include <yaml-cpp/yaml.h>

DoxyparseResults::DoxyparseResults()
{
  is_c_code = true;
}

DoxyparseResults::~DoxyparseResults() {}


void DoxyparseResults::listSymbols()
{
  // iterate over the input files
  FileNameListIterator fnli(*Doxygen::inputNameList);
  FileName *fn;

  detectProgrammingLanguage(fnli);

  // for each file
  for (fnli.toFirst(); (fn=fnli.current()); ++fnli) {
    FileNameIterator fni(*fn);
    FileDef *fd;
    yaml << YAML::BeginMap;
    for (; (fd=fni.current()); ++fni) {
      printFile(fd->absFilePath().data());
      yaml << YAML::BeginMap;
      MemberList *ml = fd->getMemberList(MemberListType_allMembersList);
      if (ml && ml->count() > 0) {
        printModule(fd->getOutputFileBase().data());
        listMembers(ml);
      }

      ClassSDict *classes = fd->getClassSDict();
      if (classes) {
        ClassSDict::Iterator cli(*classes);
        ClassDef *cd;
        for (cli.toFirst(); (cd = cli.current()); ++cli) {
          classInformation(cd);
        }
      }
      yaml << YAML::EndMap;
    }
    yaml << YAML::EndMap;
  }
  printf("%s\n", yaml.c_str());
  // TODO print external symbols referenced
}

/* Detects the programming language of the project. Actually, we only care
 * about whether it is a C project or not. */
void DoxyparseResults::detectProgrammingLanguage(FileNameListIterator& fnli) {
  FileName* fn;
  for (fnli.toFirst(); (fn=fnli.current()); ++fnli) {
    std::string filename = fn->fileName();
    if (
        checkLanguage(filename, ".cc") ||
        checkLanguage(filename, ".cxx") ||
        checkLanguage(filename, ".cpp") ||
        checkLanguage(filename, ".java") ||
        checkLanguage(filename, ".py") ||
        checkLanguage(filename, ".pyw") ||
        checkLanguage(filename, ".cs")
       ) {
      is_c_code = false;
    }
  }
}

bool DoxyparseResults::checkLanguage(std::string& filename, std::string extension) {
  if (filename.find(extension, filename.size() - extension.size()) != std::string::npos) {
    return true;
  } else {
    return false;
  }
}

void DoxyparseResults::printFile(std::string file) {
  yaml << YAML::Key << file << YAML::Value;
  // printf("%s:\n", file.c_str());
}

void DoxyparseResults::printModule(std::string module) {
  current_module = module;
  yaml << YAML::Key << module << YAML::Value;

  // printf("  %s:\n", module.c_str());
}

void DoxyparseResults::listMembers(MemberList *ml) {
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef *md;
    yaml << YAML::BeginMap;
    printDefines();
    yaml << YAML::BeginSeq;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      yaml << YAML::BeginMap;
      lookupSymbol((Definition*) md);
      yaml << YAML::EndMap;
    }
    yaml << YAML::EndSeq;
    yaml << YAML::EndMap;
  }
}

void DoxyparseResults::printDefines() {
  if (! modules[current_module]) {
    yaml << YAML::Key << "defines" << YAML::Value;
    // printf("    defines:\n");
  }
  modules[current_module] = true;
}

void DoxyparseResults::lookupSymbol(Definition *d) {
  if (d->definitionType() == Definition::TypeMember) {
    MemberDef *md = (MemberDef *)d;
    std::string type = md->memberTypeName().data();
    std::string signature = functionSignature(md);
    printDefinition(type, signature, md->getDefLine(), d);
  }
}

void DoxyparseResults::printDefinition(std::string type,
                                       std::string signature,
                                       int line,
                                       Definition *d) {
  MemberDef *md = (MemberDef *)d;
  yaml << YAML::Key << signature << YAML::Value;
  yaml << YAML::BeginMap;
  yaml << YAML::Key << "type" << YAML::Value << type;
  yaml << YAML::Key << "line" << YAML::Value << line;
  if (md->protection() == Public) {
    yaml << YAML::Key << "protection" << YAML::Value << "public";
    // printProtection("public");
  }
  if (md->isFunction()) {
    functionInformation(md);
  }
  yaml << YAML::EndMap;
  // printf("      - \"%s\":\n", signature.c_str());
  // printf("          type: %s\n", type.c_str());
  // printf("          line: %d\n", line);
}

void DoxyparseResults::printProtection(std::string protection) {
  printf("          protection: %s\n", protection.c_str());
}

void DoxyparseResults::functionInformation(MemberDef* md) {
  int size = md->getEndBodyLine() - md->getStartBodyLine() + 1;
  printNumberOfLines(size);
  ArgumentList *argList = md->argumentList();
  printNumberOfArguments(argList->count());
  printNumberOfConditionalPaths(md);
  MemberSDict *defDict = md->getReferencesMembers();
// printf("#%s\n", defDict);
  if (defDict) {
    MemberSDict::Iterator msdi(*defDict);
    MemberDef *rmd;
    printUses();
    yaml << YAML::BeginSeq;
    for (msdi.toFirst(); (rmd=msdi.current()); ++msdi) {
      yaml << YAML::BeginMap;
      if (rmd->definitionType() == Definition::TypeMember && !ignoreStaticExternalCall(md, rmd)) {
        referenceTo(rmd);
      }
      yaml << YAML::EndMap;
    }
    yaml << YAML::EndSeq;
  }
}

void DoxyparseResults::printNumberOfLines(int lines) {
  yaml << YAML::Key << "lines_of_code" << YAML::Value << lines;
  // printf("          lines_of_code: %d\n", lines);
}

void DoxyparseResults::printNumberOfArguments(int arguments) {
  yaml << YAML::Key << "parameters" << YAML::Value << arguments;
  // printf("          parameters: %d\n", arguments);
}

void DoxyparseResults::printUses() {
  yaml << YAML::Key << "uses" << YAML::Value;
  // printf("          uses:\n");
}

void DoxyparseResults::printReferenceTo(std::string type, std::string signature,
                      std::string defined_in) {
  yaml << YAML::Key << signature << YAML::Value;
  yaml << YAML::BeginMap;
  yaml << YAML::Key << "type" << YAML::Value << type;
  yaml << YAML::Key << "defined_in" << YAML::Value << defined_in;
  yaml << YAML::EndMap;
  // printf("            - \"%s\":\n", signature.c_str());
  // printf("                type: %s\n", type.c_str());
  // printf("                defined_in: %s\n", defined_in.c_str());
}

void DoxyparseResults::printNumberOfConditionalPaths(MemberDef* md) {
  yaml << YAML::Key << "conditional_paths";
  yaml << YAML::Value << md->numberOfFlowKeyWords();
  // printf("          conditional_paths: %d\n", md->numberOfFlowKeyWords());
}

//#############################################
//#############################################
//#############################################
//#############################################
//#############################################
//#############################################
bool DoxyparseResults::ignoreStaticExternalCall(MemberDef *context, MemberDef *md) {
  if (md->isStatic()) {
    if(md->getFileDef()) {
      if(md->getFileDef()->getOutputFileBase() == context->getFileDef()->getOutputFileBase())
        // TODO ignore prefix of file
        return false;
      else
        return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}
void DoxyparseResults::referenceTo(MemberDef* md) {
  std::string type = md->memberTypeName().data();
  std::string defined_in = "";
  std::string signature = "";
  if (isPartOfCStruct(md)) {
    signature = md->getClassDef()->name().data() + std::string("::") + functionSignature(md);
    defined_in = md->getClassDef()->getFileDef()->getOutputFileBase().data();
  }
  else {
    signature = functionSignature(md);
    if (md->getClassDef()) {
      defined_in = md->getClassDef()->name().data();
    }
    else if (md->getFileDef()) {
      defined_in = md->getFileDef()->getOutputFileBase().data();
    }
  }
  // printf("#-%s\n", signature.c_str());
  printReferenceTo(type, signature, defined_in);
}

int DoxyparseResults::isPartOfCStruct(MemberDef * md) {
  return is_c_code && md->getClassDef() != NULL;
}


std::string DoxyparseResults::functionSignature(MemberDef* md) {
  std::string signature = md->name().data();
  if(md->isFunction()){
    ArgumentList *argList = md->argumentList();
    ArgumentListIterator iterator(*argList);
    signature += "(";
    Argument * argument = iterator.toFirst();
    if(argument != NULL) {
      signature += argument->type.data();
      for(++iterator; (argument = iterator.current()) ;++iterator){
        signature += std::string(",") + argument->type.data();
      }
    }
    signature += ")";
  }
  return signature;
}

void DoxyparseResults::classInformation(ClassDef* cd) {
  if (is_c_code) {
    cModule(cd);
  } else {
    printModule(cd->name().data());
    BaseClassList* baseClasses = cd->baseClasses();
    if (baseClasses) {
      BaseClassListIterator bci(*baseClasses);
      BaseClassDef* bcd;
      for (bci.toFirst(); (bcd = bci.current()); ++bci) {
        printInheritance(bcd->classDef->name().data());
      }
    }
    if(cd->isAbstract()) {
      printClassInformation("abstract class");
    }
    listAllMembers(cd);
  }
}

void DoxyparseResults::cModule(ClassDef* cd) {
  MemberList* ml = cd->getMemberList(MemberListType_variableMembers);
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef* md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      printDefinition("variable", cd->name().data() + std::string("::") + md->name().data(), md->getDefLine(), md);
      // if (md->protection() == Public) {
      //   printProtection("public");
      // }
    }
  }
}

void DoxyparseResults::printInheritance(std::string base_class) {

  yaml << YAML::Key << "inherits" << YAML::Value << base_class;
  // printf("    inherits: %s\n", base_class.c_str());
}

void DoxyparseResults::printClassInformation(std::string information) {
  yaml << YAML::Key << "informations" << YAML::Value << information;
  // printf("    informations: %s\n", information.c_str());
}

void DoxyparseResults::listAllMembers(ClassDef* cd) {
  // methods
  listMembers(cd->getMemberList(MemberListType_functionMembers));
  // constructors
  listMembers(cd->getMemberList(MemberListType_constructors));
  // attributes
  listMembers(cd->getMemberList(MemberListType_variableMembers));
}
