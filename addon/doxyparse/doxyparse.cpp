#include "doxyparse.h"

Doxyparse::Doxyparse()
{
  is_c_code = true;
}

Doxyparse::~Doxyparse() {}

void Doxyparse::initializeDoxygen()
{
  // initialize data structures
  initDoxygen();
}

void Doxyparse::doxygenParseInput()
{
  parseInput();
}

void Doxyparse::setConfiguration()
{
  // check and finalize the configuration
  checkConfiguration();
  Config_getBool(MODIFY_SPECIAL_CHARS)=FALSE;
  adjustConfiguration();

  // setup the non-default configuration options

  // we need a place to put intermediate files
  tmpdir << "/tmp/doxyparse-" << getpid();
  Config_getString(OUTPUT_DIRECTORY)= tmpdir.str().c_str();
  // enable HTML (fake) output to omit warning about missing output format
  Config_getBool(GENERATE_HTML)=TRUE;
  // disable latex output
  Config_getBool(GENERATE_LATEX)=FALSE;
  // be quiet
  Config_getBool(QUIET)=TRUE;
  // turn off warnings
  Config_getBool(WARNINGS)=FALSE;
  Config_getBool(WARN_IF_UNDOCUMENTED)=FALSE;
  Config_getBool(WARN_IF_DOC_ERROR)=FALSE;
  // Extract as much as possible
  Config_getBool(EXTRACT_ALL)=TRUE;
  Config_getBool(EXTRACT_STATIC)=TRUE;
  Config_getBool(EXTRACT_PRIVATE)=TRUE;
  Config_getBool(EXTRACT_LOCAL_METHODS)=TRUE;
  // Extract source browse information, needed
  // to make doxygen gather the cross reference info
  Config_getBool(SOURCE_BROWSER)=TRUE;
  // find functions call between modules
  Config_getBool(CALL_GRAPH)=TRUE;
  // loop recursive over input files
  Config_getBool(RECURSIVE)=TRUE;
}

void Doxyparse::setInput(int argc, char **argv)
{
  // set the input
  Config_getList(INPUT).clear();
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-") == 0) {
      char filename[1024];
      while (1) {
        scanf("%s[^\n]", filename);
        if (feof(stdin)) {
          break;
        }
        Config_getList(INPUT).append(filename);
      }
    } else {
      Config_getList(INPUT).append(argv[i]);
    }
  }
  if (Config_getList(INPUT).isEmpty()) {
    exit(0);
  }
}

void Doxyparse::parseReferences()
{
  // iterate over the input files
  FileNameListIterator fnli(*Doxygen::inputNameList);
  FileName *fn;
  // for each file with a certain name
  for (fnli.toFirst();(fn=fnli.current());++fnli) {
    FileNameIterator fni(*fn);
    FileDef *fd;
    // for each file definition
    for (;(fd=fni.current());++fni) {
      // get the references (linked and unlinked) found in this file
      findXRefSymbols(fd);
    }
  }
}

void Doxyparse::removeOutputDir()
{
  // remove temporary files
  if (!Doxygen::objDBFileName.isEmpty()) unlink(Doxygen::objDBFileName);
  if (!Doxygen::entryDBFileName.isEmpty()) unlink(Doxygen::entryDBFileName);
  // clean up after us
  rmdir(Config_getString(OUTPUT_DIRECTORY));
}

void Doxyparse::removeTemporaryDir()
{
  std::string cleanup_command = "rm -rf ";
  cleanup_command += tmpdir.str();
  system(cleanup_command.c_str());
}

void Doxyparse::findXRefSymbols(FileDef *fd)
{
  // get the interface to a parser that matches the file extension
  ParserInterface *pIntf=Doxygen::parserManager->getParser(fd->getDefFileExtension());

  // get the programming language from the file name
  SrcLangExt lang = getLanguageFromFileName(fd->name());

  // reset the parsers state
  pIntf->resetCodeParserState();

  // create a new backend object
  DoxyparseInterface *parse = new DoxyparseInterface(fd);

  // parse the source code
  pIntf->parseCode(*parse, 0, fileToString(fd->absFilePath()), lang, FALSE, 0, fd);

  // dismiss the object.
  delete parse;
}

bool Doxyparse::ignoreStaticExternalCall(MemberDef *context, MemberDef *md)
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

void Doxyparse::printArgumentList(MemberDef *md)
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

void Doxyparse::printType(MemberDef *md)
{
  printf("%s ", md->memberTypeName().data());
}

void Doxyparse::printSignature(MemberDef* md)
{
  printf("%s", md->name().data());
  if(md->isFunction()){
    printArgumentList(md);
  }
  printf(" ");
}

void Doxyparse::printWhereItWasDefined(MemberDef *md)
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

void Doxyparse::printCStructMember(MemberDef *md)
{
  printType(md);
  printf("%s::", md->getClassDef()->name().data());
  printSignature(md);
  printf("defined in %s\n", md->getClassDef()->getFileDef()->getOutputFileBase().data());
}

int Doxyparse::isPartOfCStruct(MemberDef *md)
{
  return is_c_code && md->getClassDef() != NULL;
}

void Doxyparse::printReferenceTo(MemberDef *md)
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

void Doxyparse::printReferencesMembers(MemberDef *md)
{
  MemberSDict *defDict = md->getReferencesMembers();
  if (defDict) {
    MemberSDict::Iterator msdi(*defDict);
    MemberDef *rmd;
    for (msdi.toFirst(); (rmd=msdi.current()); ++msdi) {
      if (rmd->definitionType() == Definition::TypeMember && !ignoreStaticExternalCall(md, rmd)) {
        printReferenceTo(rmd);
      }
    }
  }
}

void Doxyparse::printDefinitionLine(MemberDef *md)
{
  printf("in line %d\n", md->getDefLine());
}

void Doxyparse::printDefinition(MemberDef *md)
{
  printf("   ");
  printType(md);
  printSignature(md);
  printDefinitionLine(md);
}

void Doxyparse::printProtection(MemberDef *md)
{
  if (md->protection() == Public) {
    printf("      protection public\n");
  }
}

void Doxyparse::printNumberOfLines(MemberDef *md)
{
  int size = md->getEndBodyLine() - md->getStartBodyLine() + 1;
  printf("      %d lines of code\n", size);
}

void Doxyparse::printNumberOfArguments(MemberDef *md)
{
  ArgumentList *argList = md->argumentList();
  printf("      %d parameters\n", argList->count());
}

void Doxyparse::printNumberOfConditionalPaths(MemberDef *md)
{
  printf("      %d conditional paths\n", md->numberOfFlowKeyWords());
}

void Doxyparse::printFunctionInformation(MemberDef *md)
{
  printNumberOfLines(md);
  printNumberOfArguments(md);
  printNumberOfConditionalPaths(md);
  printReferencesMembers(md);
}

void Doxyparse::lookupSymbol(Definition *d)
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

void Doxyparse::listMembers(MemberList *ml)
{
  if (ml) {
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst(); (md=mli.current()); ++mli) {
      lookupSymbol((Definition*) md);
    }
  }
}

void Doxyparse::printInheritance(ClassDef *cd)
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

void Doxyparse::printCModule(ClassDef *cd)
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

void Doxyparse::listAllMembers(ClassDef *cd)
{
  // methods
  listMembers(cd->getMemberList(MemberListType_functionMembers));
  // constructors
  listMembers(cd->getMemberList(MemberListType_constructors));
  // attributes
  listMembers(cd->getMemberList(MemberListType_variableMembers));
}

void Doxyparse::printClassInformation(ClassDef *cd)
{
  printf("module %s\n", cd->name().data());
  printInheritance(cd);
  if(cd->isAbstract()) {
    printf("   abstract class\n");
  }
  listAllMembers(cd);
}

void Doxyparse::printClass(ClassDef *cd)
{
  if (is_c_code) {
    printCModule(cd);
  } else {
    printClassInformation(cd);
  }
}

void Doxyparse::printFile(FileDef *fd)
{
  printf("file %s\n", fd->absFilePath().data());
  MemberList *ml = fd->getMemberList(MemberListType_allMembersList);
  if (ml && ml->count() > 0) {
    printf("module %s\n", fd->getOutputFileBase().data());
    listMembers(ml);
  }
}

bool Doxyparse::checkLanguage(std::string &filename, std::string extension)
{
  if (filename.find(extension, filename.size() - extension.size()) != std::string::npos) {
    return true;
  } else {
    return false;
  }
}

/* Detects the programming language of the project. Actually, we only care
 * about whether it is a C project or not. */
void Doxyparse::detectProgrammingLanguage(FileNameListIterator &fnli)
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

void Doxyparse::listSymbols()
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
