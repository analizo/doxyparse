#include "doxyparseresults.h"
#include <yaml-cpp/yaml.h>

DoxyparseResults::DoxyparseResults()
{
  is_c_code = true;
}

DoxyparseResults::~DoxyparseResults() {}

FileNameListIterator DoxyparseResults::getFiles()
{
    FileNameListIterator file_namelist_iterator(*Doxygen::inputNameList);
    return file_namelist_iterator;
}

void DoxyparseResults::verifyEmptyMemberListOrClasses(MemberList *member_list, ClassSDict *classes, enum YAML::EMITTER_MANIP value){
    if (member_list && member_list->count() > 0 || classes){
      *this->yaml << value;
  } else if(value == YAML::BeginMap) {
      *this->yaml << "~";
  } else{
      // Nothing to do.
  }
}

void DoxyparseResults::listSymbols()
{
  FileNameListIterator file_namelist_iterator = this->getFiles();
  this->detectIsCCode(file_namelist_iterator);
  file_namelist_iterator.toFirst();

  for (FileName *file_name; (file_name=file_namelist_iterator.current()); ++file_namelist_iterator) {
    FileNameIterator file_name_iterator(*file_name);

    this->yaml = new YAML::Emitter();
    *this->yaml << YAML::BeginMap;

    for (FileDef *file_definition; (file_definition=file_name_iterator.current()); ++file_name_iterator) {
      addKeyYaml(file_definition->absFilePath().data()); //Print file

      ClassSDict *classes = file_definition->getClassSDict();
      MemberList *member_list = file_definition->getMemberList(MemberListType_allMembersList);

      this->verifyEmptyMemberListOrClasses(member_list, classes, YAML::BeginMap);

      if (member_list && member_list->count() > 0) {
        addValue(file_definition->getOutputFileBase().data()); //Print module
        addValue(DEFINES, YAML::BeginMap); //Print defines
        *this->yaml << YAML::BeginSeq;
        listMembers(member_list);

        if (classes && is_c_code) {
          *this->yaml << YAML::BeginMap;
          ClassSDict::Iterator cli(*classes);
          ClassDef *cd;
          for (cli.toFirst(); (cd = cli.current()); ++cli) {
            classInformation(cd);
          }
          *this->yaml << YAML::EndMap;
        }
        *this->yaml << YAML::EndSeq;
        *this->yaml << YAML::EndMap;
      }

      if (classes && !is_c_code) {
        ClassSDict::Iterator cli(*classes);
        ClassDef *cd;
        for (cli.toFirst(); (cd = cli.current()); ++cli) {
          classInformation(cd);
        }
      }

      this->verifyEmptyMemberListOrClasses(member_list, classes, YAML::EndMap);
    }
    *this->yaml << YAML::EndMap;
    printf("%s\n", (*this->yaml).c_str());
    delete this->yaml;
  }
  // TODO print external symbols referenced
}

/* Detects the programming language of the project. Actually, we only care
 * about whether it is a C project or not. */
void DoxyparseResults::detectIsCCode(FileNameListIterator& file_namelist_iterator) {
  FileName* fn;
  for (file_namelist_iterator.toFirst(); (fn=file_namelist_iterator.current()); ++file_namelist_iterator) {
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

void DoxyparseResults::addKeyYaml(std::string key) {
  *this->yaml << YAML::Key << key;
}

void DoxyparseResults::addValue(std::string key, enum YAML::EMITTER_MANIP value) {
  *this->yaml << value;
  this->addValue(key);
}

void DoxyparseResults::addValue(std::string key, std::string value) {
  addKeyYaml(key);
  *this->yaml << YAML::Value << value;
}

void DoxyparseResults::addValue(std::string key, int value){
  addValue(key, std::to_string(value));
}

void DoxyparseResults::addValue(std::string key) {
  addKeyYaml(key);
  *this->yaml << YAML::Value;
}

//TODO: DELETE METHOD
void DoxyparseResults::printFile(std::string file) {
  *this->yaml << YAML::Key << file << YAML::Value;
}

//TODO: DELETE METHOD
void DoxyparseResults::printModule(std::string module) {
  current_module = module;
  *yaml << YAML::Key << module << YAML::Value;
}

void DoxyparseResults::listMembers(MemberList *ml) {
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      *yaml << YAML::BeginMap;
      lookupSymbol((Definition*) md);
      *yaml << YAML::EndMap;
    }
  }
}

//TODO: DELETE METHOD
void DoxyparseResults::printDefines() {
  *yaml << YAML::Key << DEFINES << YAML::Value;
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
  *yaml << YAML::Key << YAML::DoubleQuoted << signature << YAML::Value;
  *yaml << YAML::BeginMap;
  addValue(TYPE, type); //Print type
  addValue(LINE, line); //Print number line
  if (md->protection() == Public) {
    addValue(PROTECTION, PUBLIC);
  }
  if (md->isFunction()) {
    functionInformation(md);
  }
  *yaml << YAML::EndMap;
}

void DoxyparseResults::functionInformation(MemberDef* md) {
  int size = md->getEndBodyLine() - md->getStartBodyLine() + 1;
  addValue(LINES_OF_CODE, size); //Print number of lines
  ArgumentList *argList = md->argumentList();
  addValue(PARAMETERS, argList->count()); //Print number of arguments
  addValue(CONDITIONAL_PATHS, md->numberOfFlowKeyWords()); //Print number of conditional paths
  MemberSDict *defDict = md->getReferencesMembers();
  if (defDict) {
    MemberSDict::Iterator msdi(*defDict);
    MemberDef *rmd;
    addValue(USES); //Print uses
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

//TODO: DELETE METHOD
void DoxyparseResults::printNumberOfLines(int lines) {
  *yaml << YAML::Key << LINES_OF_CODE << YAML::Value << lines;
}

//TODO: DELETE METHOD
void DoxyparseResults::printNumberOfArguments(int arguments) {
  *yaml << YAML::Key << PARAMETERS << YAML::Value << arguments;
}

//TODO: DELETE METHOD
void DoxyparseResults::printUses() {
  *yaml << YAML::Key << USES << YAML::Value;
}

void DoxyparseResults::printReferenceTo(std::string type, std::string signature,
                      std::string defined_in) {
  *yaml << YAML::Key << YAML::DoubleQuoted << signature << YAML::Value;
  *yaml << YAML::BeginMap;
  addValue(TYPE, type); //Print type
  addValue(DEFINED_IN, defined_in); //Print line definition
  *yaml << YAML::EndMap;
}

//TODO: DELETE METHOD
void DoxyparseResults::printNumberOfConditionalPaths(MemberDef* md) {
  *yaml << YAML::Key << CONDITIONAL_PATHS;
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
    addValue(cd->name().data()); //Print module name
    BaseClassList* baseClasses = cd->baseClasses();
    *yaml << YAML::BeginMap;
    if (baseClasses) {
      BaseClassListIterator bci(*baseClasses);
      BaseClassDef* bcd;
      for (bci.toFirst(); (bcd = bci.current()); ++bci) {
        addValue(INHERITS, bcd->classDef->name().data()); //Print inherits
      }
    }
    if(cd->isAbstract()) {
      addValue(INFORMATIONS, ABSTRACT_CLASS); //Print class information
    }
    addValue(DEFINES); //Print defines
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
      printDefinition(VARIABLE, cd->name().data() + std::string("::") + md->name().data(), md->getDefLine(), md);
    }
  }
}

//TODO: DELETE METHOD
void DoxyparseResults::printInheritance(std::string base_class) {

  *yaml << YAML::Key << INHERITS << YAML::Value << base_class;
}


//TODO: DELETE METHOD
void DoxyparseResults::printClassInformation(std::string information) {
  *yaml << YAML::Key << INFORMATIONS << YAML::Value << information;
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
