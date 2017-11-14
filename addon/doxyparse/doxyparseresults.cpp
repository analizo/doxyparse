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
    yaml = new YAML::Emitter();
    *yaml << YAML::BeginMap;
    for (; (fd=fni.current()); ++fni) {
      printFile(fd->absFilePath().data());
      ClassSDict *classes = fd->getClassSDict();
      MemberList *ml = fd->getMemberList(MemberListType_allMembersList);
      if (ml && ml->count() > 0 || classes){
        *yaml << YAML::Value << YAML::BeginMap;
      } else {
        *yaml << YAML::Value << "~";
      }

      bool has_struct = false;
      if (ml && ml->count() > 0) {
        printModule(fd->getOutputFileBase().data());
        *yaml << YAML::BeginMap;
        printDefines();
        *yaml << YAML::BeginSeq;
        listMembers(ml);
        if (classes && is_c_code) {
          *yaml << YAML::BeginMap;
          has_struct = true;
          ClassSDict::Iterator cli(*classes);
          ClassDef *cd;
          for (cli.toFirst(); (cd = cli.current()); ++cli) {
            classInformation(cd);
          }
          *yaml << YAML::EndMap;
        }
        *yaml << YAML::EndSeq;
        *yaml << YAML::EndMap;
      }

      if (classes && !has_struct) {
        ClassSDict::Iterator cli(*classes);
        ClassDef *cd;
        for (cli.toFirst(); (cd = cli.current()); ++cli) {
          classInformation(cd);
        }
      }

      if (ml && ml->count() > 0 || classes){
        *yaml << YAML::EndMap;
      }
    }
    *yaml << YAML::EndMap;
    printf("%s\n", (*yaml).c_str());
    delete yaml;
  }
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
  *yaml << YAML::Key << file;
}

void DoxyparseResults::printModule(std::string module) {
  current_module = module;
  *yaml << YAML::Key << module << YAML::Value;
}

void DoxyparseResults::listMembers(MemberList *ml) {
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      if (md->definitionType() == Definition::TypeMember) {
        *yaml << YAML::BeginMap;
        lookupSymbol((Definition*) md);
        *yaml << YAML::EndMap;
      }
    }
  }
}

void DoxyparseResults::printDefines() {
  *yaml << YAML::Key << "defines" << YAML::Value;
  modules[current_module] = true;
}

void DoxyparseResults::lookupSymbol(Definition *d) {
    MemberDef *md = (MemberDef *)d;
    std::string type = md->memberTypeName().data();
    std::string signature = functionSignature(md);
    printDefinition(type, signature, md->getDefLine(), d);
}

void DoxyparseResults::printDefinition(std::string type,
                                       std::string signature,
                                       int line,
                                       Definition *d) {
  MemberDef *md = (MemberDef *)d;
  *yaml << YAML::Key << YAML::DoubleQuoted << signature << YAML::Value;
  *yaml << YAML::BeginMap;
  *yaml << YAML::Key << "type" << YAML::Value << type;
  *yaml << YAML::Key << "line" << YAML::Value << line;
  if (md->protection() == Public) {
    *yaml << YAML::Key << "protection" << YAML::Value << "public";
  }
  if (md->isFunction()) {
    functionInformation(md);
  }
  *yaml << YAML::EndMap;
}

void DoxyparseResults::functionInformation(MemberDef* md) {
  int size = md->getEndBodyLine() - md->getStartBodyLine() + 1;
  printNumberOfLines(size);
  ArgumentList *argList = md->argumentList();
  printNumberOfArguments(argList->count());
  printNumberOfConditionalPaths(md);
  MemberSDict *defDict = md->getReferencesMembers();
  if (defDict) {
    MemberSDict::Iterator msdi(*defDict);
    MemberDef *rmd;
    printUses();
    *yaml << YAML::BeginSeq;
    for (msdi.toFirst(); (rmd=msdi.current()); ++msdi) {
      if (rmd->definitionType() == Definition::TypeMember && !ignoreStaticExternalCall(md, rmd)) {
        *yaml << YAML::BeginMap;
        referenceTo(rmd);
        *yaml << YAML::EndMap;
      }
    }
    *yaml << YAML::EndSeq;
  }
}

void DoxyparseResults::printNumberOfLines(int lines) {
  *yaml << YAML::Key << "lines_of_code" << YAML::Value << lines;
}

void DoxyparseResults::printNumberOfArguments(int arguments) {
  *yaml << YAML::Key << "parameters" << YAML::Value << arguments;
}

void DoxyparseResults::printUses() {
  *yaml << YAML::Key << "uses" << YAML::Value;
}

void DoxyparseResults::printReferenceTo(std::string type, std::string signature,
                      std::string defined_in) {
  *yaml << YAML::Key << YAML::DoubleQuoted << signature << YAML::Value;
  *yaml << YAML::BeginMap;
  *yaml << YAML::Key << "type" << YAML::Value << type;
  *yaml << YAML::Key << "defined_in" << YAML::Value << defined_in;
  *yaml << YAML::EndMap;
}

void DoxyparseResults::printNumberOfConditionalPaths(MemberDef* md) {
  *yaml << YAML::Key << "conditional_paths";
  *yaml << YAML::Value << md->numberOfFlowKeyWords();
}

bool DoxyparseResults::ignoreStaticExternalCall(MemberDef *context, MemberDef *md) {
  return md->isStatic() &&
         md->getFileDef() &&
         !(md->getFileDef()->getOutputFileBase() == context->getFileDef()->getOutputFileBase());
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
    *yaml << YAML::BeginMap;
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
    printDefines();
    listAllMembers(cd);
    *yaml << YAML::EndMap;
  }
}

void DoxyparseResults::cModule(ClassDef* cd) {
  MemberList* ml = cd->getMemberList(MemberListType_variableMembers);
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef* md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      printDefinition("variable", cd->name().data() + std::string("::") + md->name().data(), md->getDefLine(), md);
    }
  }
}

void DoxyparseResults::printInheritance(std::string base_class) {

  *yaml << YAML::Key << "inherits" << YAML::Value << base_class;
}

void DoxyparseResults::printClassInformation(std::string information) {
  *yaml << YAML::Key << "informations" << YAML::Value << information;
}

void DoxyparseResults::listAllMembers(ClassDef* cd) {
  *yaml << YAML::BeginSeq;
  // methods
  listMembers(cd->getMemberList(MemberListType_functionMembers));
  // constructors
  listMembers(cd->getMemberList(MemberListType_constructors));
  // attributes
  listMembers(cd->getMemberList(MemberListType_variableMembers));
  *yaml << YAML::EndSeq;
}
