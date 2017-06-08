#include "doxyparser.h"

Doxyparser::Doxyparser(){}
Doxyparser::~Doxyparser(){}

void Doxyparser::parseReferences()
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

void Doxyparser::findXRefSymbols(FileDef *fd)
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
