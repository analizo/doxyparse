#include "doxyparseresults.h"

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
    for (; (fd=fni.current()); ++fni) {
      printFile(fd);

      ClassSDict *classes = fd->getClassSDict();
      if (classes) {
        ClassSDict::Iterator cli(*classes);
        ClassDef *cd;
        for (cli.toFirst(); (cd = cli.current()); ++cli) {
          printClass(cd);
        }
      }
    }
  }
  // TODO print external symbols referenced
}

/* Detects the programming language of the project. Actually, we only care
 * about whether it is a C project or not. */
void DoxyparseResults::detectProgrammingLanguage(FileNameListIterator &fnli)
{
  FileName* fn;
  for (fnli.toFirst(); (fn=fnli.current()); ++fnli) {
    std::string filename = fn->fileName();
    if (
        checkLanguage(filename, ".cc") ||
        checkLanguage(filename, ".cxx") ||
        checkLanguage(filename, ".cpp") ||
        checkLanguage(filename, ".java") ||
        checkLanguage(filename, ".py") ||
        checkLanguage(filename, ".pyw")
       ) {
      is_c_code = false;
    }
  }
}

bool DoxyparseResults::checkLanguage(std::string &filename, std::string extension)
{
  if (filename.find(extension, filename.size() - extension.size()) != std::string::npos) {
    return true;
  } else {
    return false;
  }
}

void DoxyparseResults::printFile(FileDef *fd)
{
  printf("file %s\n", fd->absFilePath().data());
  MemberList *ml = fd->getMemberList(MemberListType_allMembersList);
  if (ml && ml->count() > 0) {
    printf("module %s\n", fd->getOutputFileBase().data());
    listMembers(ml);
  }
}

void DoxyparseResults::listMembers(MemberList *ml)
{
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      lookupSymbol((Definition*) md);
    }
  }
}

void DoxyparseResults::lookupSymbol(Definition *d)
{
  if (d->definitionType() == Definition::TypeMember) {
    MemberDef *md = (MemberDef *)d;
    printDefinition(md);
    printProtection(md);
    if (md->isFunction()) {
      printFunctionInformation(md);
    }
  }
}

void DoxyparseResults::printDefinition(MemberDef *md)
{
  printf("   ");
  printType(md);
  printSignature(md);
  printDefinitionLine(md);
}

void DoxyparseResults::printType(MemberDef *md)
{
  printf("%s ", md->memberTypeName().data());
}

void DoxyparseResults::printSignature(MemberDef* md)
{
  printf("%s", md->name().data());
  if(md->isFunction()){
    printArgumentList(md);
  }
  printf(" ");
}

void DoxyparseResults::printArgumentList(MemberDef *md)
{
  ArgumentList *argList = md->argumentList();
  ArgumentListIterator iterator(*argList);

  printf("(");
  Argument * argument = iterator.toFirst();
  if(argument != NULL) {
    printf("%s", argument->type.data());
    for(++iterator; (argument = iterator.current()) ;++iterator){
      printf(",%s", argument->type.data());
    }
  }
  printf(")");
}

void DoxyparseResults::printDefinitionLine(MemberDef *md)
{
  printf("in line %d\n", md->getDefLine());
}

void DoxyparseResults::printProtection(MemberDef *md)
{
  if (md->protection() == Public) {
    printf("      protection public\n");
  }
}

void DoxyparseResults::printFunctionInformation(MemberDef *md)
{
  printNumberOfLines(md);
  printNumberOfArguments(md);
  printNumberOfConditionalPaths(md);
  printReferencesMembers(md);
}

void DoxyparseResults::printNumberOfLines(MemberDef *md)
{
  int size = md->getEndBodyLine() - md->getStartBodyLine() + 1;
  printf("      %d lines of code\n", size);
}

void DoxyparseResults::printNumberOfArguments(MemberDef *md)
{
  ArgumentList *argList = md->argumentList();
  printf("      %d parameters\n", argList->count());
}

void DoxyparseResults::printNumberOfConditionalPaths(MemberDef *md)
{
  printf("      %d conditional paths\n", md->numberOfFlowKeyWords());
}

void DoxyparseResults::printReferencesMembers(MemberDef *md)
{
  MemberSDict *defDict = md->getReferencesMembers();
  if (defDict) {
    MemberSDict::Iterator msdi(*defDict);
    MemberDef *rmd;
    for (msdi.toFirst(); (rmd=msdi.current()); ++msdi) {
      if (rmd->definitionType() == Definition::TypeMember
          && !ignoreStaticExternalCall(md, rmd)) {
        printReferenceTo(rmd);
      }
    }
  }
}

bool DoxyparseResults::ignoreStaticExternalCall(MemberDef *context, MemberDef *md)
{
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

void DoxyparseResults::printReferenceTo(MemberDef *md)
{
  printf("      uses ");
  if (isPartOfCStruct(md)) {
    printCStructMember(md);
  }
  else {
    printType(md);
    printSignature(md);
    printWhereItWasDefined(md);
  }
}

int DoxyparseResults::isPartOfCStruct(MemberDef *md)
{
  return is_c_code && md->getClassDef() != NULL;
}

void DoxyparseResults::printCStructMember(MemberDef *md)
{
  printType(md);
  printf("%s::", md->getClassDef()->name().data());
  printSignature(md);
  printf("defined in %s\n", md->getClassDef()->getFileDef()->getOutputFileBase().data());
}

void DoxyparseResults::printWhereItWasDefined(MemberDef *md)
{
  if (md->getClassDef()) {
    printf("defined in %s\n", md->getClassDef()->name().data());
  }
  else if (md->getFileDef()) {
    printf("defined in %s\n", md->getFileDef()->getOutputFileBase().data());
  }
  else {
    printf("\n");
  }
}

void DoxyparseResults::printClass(ClassDef *cd)
{
  if (is_c_code) {
    printCModule(cd);
  } else {
    printClassInformation(cd);
  }
}

void DoxyparseResults::printCModule(ClassDef *cd)
{
  MemberList* ml = cd->getMemberList(MemberListType_variableMembers);
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef* md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      printf("   variable %s::%s in line %d\n", cd->name().data(), md->name().data(), md->getDefLine());
      printProtection(md);
    }
  }
}

void DoxyparseResults::printClassInformation(ClassDef *cd)
{
  printf("module %s\n", cd->name().data());
  printInheritance(cd);
  if(cd->isAbstract()) {
    printf("   abstract class\n");
  }
  listAllMembers(cd);
}

void DoxyparseResults::printInheritance(ClassDef *cd)
{
  BaseClassList* baseClasses = cd->baseClasses();
  if (baseClasses) {
    BaseClassListIterator bci(*baseClasses);
    BaseClassDef* bcd;
    for (bci.toFirst(); (bcd = bci.current()); ++bci) {
      printf("   inherits from %s\n", bcd->classDef->name().data());
    }
  }
}

void DoxyparseResults::listAllMembers(ClassDef *cd)
{
  // methods
  listMembers(cd->getMemberList(MemberListType_functionMembers));
  // constructors
  listMembers(cd->getMemberList(MemberListType_constructors));
  // attributes
  listMembers(cd->getMemberList(MemberListType_variableMembers));
}
