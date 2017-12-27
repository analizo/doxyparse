#include "doxyparseresults.h"
#include <yaml-cpp/yaml.h>

DoxyparseResults::DoxyparseResults()
{
  this->is_c_code = true;
}

DoxyparseResults::~DoxyparseResults() {}

FileNameListIterator DoxyparseResults::getFiles()
{
    FileNameListIterator file_name_list_iterator(*Doxygen::inputNameList);
    return file_name_list_iterator;
}

void DoxyparseResults::verifyEmptyMemberListOrClasses(MemberList *member_list, ClassSDict *classes, enum YAML::EMITTER_MANIP value){
    if (member_list && member_list->count() > 0 || classes){
      *this->yaml << value;
  } else if(value == YAML::BeginMap) {
      *this->yaml << "~";
  }
}

void DoxyparseResults::listSymbols()
{
  FileNameListIterator file_name_list_iterator = this->getFiles();
  this->detectIsCCode(file_name_list_iterator);
  file_name_list_iterator.toFirst();

  for (FileName *file_name; (file_name=file_name_list_iterator.current()); ++file_name_list_iterator) {
    FileNameIterator file_name_iterator(*file_name);

    this->yaml = new YAML::Emitter();
    *this->yaml << YAML::BeginMap;

    for (FileDef *file_def; (file_def=file_name_iterator.current()); ++file_name_iterator) {
      this->addKeyYaml(file_def->absFilePath().data()); //Print file

      ClassSDict *classes = file_def->getClassSDict();
      MemberList *member_list = file_def->getMemberList(MemberListType_allMembersList);

      this->verifyEmptyMemberListOrClasses(member_list, classes, YAML::BeginMap);

      this->loadFileMembersIntoYaml(member_list, file_def, classes);

      this->verifyEmptyMemberListOrClasses(member_list, classes, YAML::EndMap);
    }
    *this->yaml << YAML::EndMap;
    printf("%s\n", (*this->yaml).c_str());
    delete this->yaml;
  }
  // TODO print external symbols referenced
}

void DoxyparseResults::loadFileMembersIntoYaml(MemberList *member_list, FileDef *file_def, ClassSDict *classes) {
  if (member_list) {

    this->addValue(file_def->getOutputFileBase().data());
    this->addValue(DEFINES, YAML::BeginMap);
    *this->yaml << YAML::BeginSeq;
    this->listMembers(member_list);

    if (classes && this->is_c_code) {
      *this->yaml << YAML::BeginMap;
      ClassSDict::Iterator class_element_iterator(*classes);
      ClassDef *class_def;
      for (class_element_iterator.toFirst(); (class_def = class_element_iterator.current()); ++class_element_iterator) {
        this->cModule(class_def);
      }
      *this->yaml << YAML::EndMap;
    }
    *this->yaml << YAML::EndSeq;
    *this->yaml << YAML::EndMap;
  }

  if (classes && !this->is_c_code) {
    ClassSDict::Iterator class_element_iterator(*classes);
    ClassDef *class_def;
    for (class_element_iterator.toFirst(); (class_def = class_element_iterator.current()); ++class_element_iterator) {
      this->classInformation(class_def);
    }
  }
}

/* Detects the programming language of the project. Actually, we only care
 * about whether it is a C project or not. */
void DoxyparseResults::detectIsCCode(FileNameListIterator& file_name_list_iterator) {
  FileName* file_name;
  for (file_name_list_iterator.toFirst(); (file_name=file_name_list_iterator.current()); ++file_name_list_iterator) {
    std::string filename = file_name->fileName();
    if (
        this->checkLanguage(filename, ".cc") ||
        this->checkLanguage(filename, ".cxx") ||
        this->checkLanguage(filename, ".cpp") ||
        this->checkLanguage(filename, ".java") ||
        this->checkLanguage(filename, ".py") ||
        this->checkLanguage(filename, ".pyw") ||
        this->checkLanguage(filename, ".cs")
       ) {
      this->is_c_code = false;
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
  this->addKeyYaml(key);
  *this->yaml << YAML::Value << value;
}

void DoxyparseResults::addValue(std::string key, int value){
  this->addValue(key, std::to_string(value));
}

void DoxyparseResults::addValue(std::string key) {
  this->addKeyYaml(key);
  *this->yaml << YAML::Value;
}

void DoxyparseResults::listMembers(MemberList *member_list) {
  if (member_list) {
    MemberListIterator member_list_iterator(*member_list);
    MemberDef *member_def;
    for (member_list_iterator.toFirst(); (member_def=member_list_iterator.current()); ++member_list_iterator) {
      *this->yaml << YAML::BeginMap;
      this->lookupSymbol((Definition*) member_def);
      *this->yaml << YAML::EndMap;
    }
  }
}

void DoxyparseResults::lookupSymbol(Definition *def) {
  if (def->definitionType() == Definition::TypeMember) {
    MemberDef *member_def = (MemberDef *)def;
    std::string type = member_def->memberTypeName().data();
    std::string signature = this->functionSignature(member_def);
    this->printDefinition(type, signature, member_def->getDefLine(), def);
  }
}

void DoxyparseResults::printDefinition(std::string type,
                                       std::string signature,
                                       int line,
                                       Definition *def) {
  MemberDef *member_def = (MemberDef *)def;
  *this->yaml << YAML::Key << YAML::DoubleQuoted << signature << YAML::Value;
  *this->yaml << YAML::BeginMap;
  this->addValue(TYPE, type); //Print type
  this->addValue(LINE, line); //Print number line
  if (member_def->protection() == Public) {
    this->addValue(PROTECTION, PUBLIC);
  }
  if (member_def->isFunction()) {
    this->functionInformation(member_def);
  }
  *this->yaml << YAML::EndMap;
}

void DoxyparseResults::functionInformation(MemberDef* member_def) {
  int size = member_def->getEndBodyLine() - member_def->getStartBodyLine() + 1;
  this->addValue(LINES_OF_CODE, size); //Print number of lines
  ArgumentList *argList = member_def->argumentList();
  this->addValue(PARAMETERS, argList->count()); //Print number of arguments
  this->addValue(CONDITIONAL_PATHS, member_def->numberOfFlowKeyWords()); //Print number of conditional paths
  MemberSDict *def_dict = member_def->getReferencesMembers();
  if (def_dict) {
    MemberSDict::Iterator member_dict_iterator(*def_dict);
    MemberDef *member_def;
    this->addValue(USES); //Print uses
    *this->yaml << YAML::BeginSeq;
    for (member_dict_iterator.toFirst(); (member_def=member_dict_iterator.current()); ++member_dict_iterator) {
      if (member_def->definitionType() == Definition::TypeMember && !this->ignoreStaticExternalCall(member_def, member_def)) {
        *this->yaml << YAML::BeginMap;
        this->referenceTo(member_def);
        *this->yaml << YAML::EndMap;
      }
    }
    *this->yaml << YAML::EndSeq;
  }
}

void DoxyparseResults::printReferenceTo(std::string type, std::string signature,
                      std::string defined_in) {
  *this->yaml << YAML::Key << YAML::DoubleQuoted << signature << YAML::Value;
  *this->yaml << YAML::BeginMap;
  this->addValue(TYPE, type); //Print type
  this->addValue(DEFINED_IN, defined_in); //Print line definition
  *this->yaml << YAML::EndMap;
}

bool DoxyparseResults::ignoreStaticExternalCall(MemberDef *context, MemberDef *member_def) {
  return member_def->isStatic() &&
         member_def->getFileDef() &&
         !(member_def->getFileDef()->getOutputFileBase() == context->getFileDef()->getOutputFileBase());
}

void DoxyparseResults::referenceTo(MemberDef* member_def) {
  std::string type = member_def->memberTypeName().data();
  std::string defined_in = "";
  std::string signature = "";
  if (isPartOfCStruct(member_def)) {
    signature = member_def->getClassDef()->name().data() + std::string("::") + this->functionSignature(member_def);
    defined_in = member_def->getClassDef()->getFileDef()->getOutputFileBase().data();
  }
  else {
    signature = this->functionSignature(member_def);
    if (member_def->getClassDef()) {
      defined_in = member_def->getClassDef()->name().data();
    }
    else if (member_def->getFileDef()) {
      defined_in = member_def->getFileDef()->getOutputFileBase().data();
    }
  }
  this->printReferenceTo(type, signature, defined_in);
}

int DoxyparseResults::isPartOfCStruct(MemberDef * member_def) {
  return this->is_c_code && member_def->getClassDef() != NULL;
}

std::string DoxyparseResults::functionSignature(MemberDef* member_def) {
  std::string signature = member_def->name().data();
  if(member_def->isFunction()){
    ArgumentList *argList = member_def->argumentList();
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

void DoxyparseResults::classInformation(ClassDef* class_def) {
  this->addValue(class_def->name().data());
  BaseClassList* baseClasses = class_def->baseClasses();
  *this->yaml << YAML::BeginMap;
  if (baseClasses) {
    BaseClassListIterator base_list_iterator(*baseClasses);
    BaseClassDef* base_class_def;
    for (base_list_iterator.toFirst(); (base_class_def = base_list_iterator.current()); ++base_list_iterator) {
      this->addValue(INHERITS, base_class_def->classDef->name().data());
    }
  }
  if(class_def->isAbstract()) {
    this->addValue(INFORMATIONS, ABSTRACT_CLASS);
  }
  this->addValue(DEFINES);
  this->listAllMembers(class_def);
  *this->yaml << YAML::EndMap;
}

void DoxyparseResults::cModule(ClassDef* class_def) {
  MemberList* member_list = class_def->getMemberList(MemberListType_variableMembers);
  if (member_list) {
    MemberListIterator member_list_iterator(*member_list);
    MemberDef* member_def;
    for (member_list_iterator.toFirst(); (member_def=member_list_iterator.current()); ++member_list_iterator) {
      this->printDefinition(VARIABLE, class_def->name().data() + std::string("::") + member_def->name().data(), member_def->getDefLine(), member_def);
    }
  }
}

void DoxyparseResults::listAllMembers(ClassDef* class_def) {
  *this->yaml << YAML::BeginSeq;
  // methods
  this->listMembers(class_def->getMemberList(MemberListType_functionMembers));
  // constructors
  this->listMembers(class_def->getMemberList(MemberListType_constructors));
  // attributes
  this->listMembers(class_def->getMemberList(MemberListType_variableMembers));
  *this->yaml << YAML::EndSeq;
}
