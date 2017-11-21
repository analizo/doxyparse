#include "doxyparser.h"

Doxyparser::Doxyparser(){}
Doxyparser::~Doxyparser(){}

void Doxyparser::parse_references()
{
  // iterate over the input files
  FileNameListIterator file_name_list_iterator(*Doxygen::inputNameList);
  FileName *file_name;
  // for each file with a certain name
  for (file_name_list_iterator.toFirst();(file_name=file_name_list_iterator.current());
    ++file_name_list_iterator) {

    FileNameIterator fni(*file_name);
    FileDef *fd;
    // for each file definition
    for (;(fd=fni.current());++fni) {
      // get the references (linked and unlinked) found in this file
      this->find_x_ref_symbols(fd);
    }
  }
}

void Doxyparser::find_x_ref_symbols(FileDef *file_def)
{
  // get the interface to a parser that matches the file extension
  ParserInterface *p_intf = Doxygen::parserManager->getParser(file_def->getDefFileExtension());

  // get the programming language from the file name
  SrcLangExt lang = getLanguageFromFileName(file_def->name());

  // reset the parsers state
  p_intf->resetCodeParserState();

  // create a new backend object
  DoxyparseInterface *parse = new DoxyparseInterface(file_def);

  // parse the source code
  p_intf->parseCode(*parse, 0, fileToString(file_def->absFilePath()), lang, FALSE, 0, file_def);

  // dismiss the object.
  delete parse;
}
