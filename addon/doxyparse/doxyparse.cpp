/******************************************************************************
 *
 * Copyright (C) 2009 by Joenio Costa.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

/** @file
 *  @brief Code parse based on doxyapp by Dimitri van Heesch
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include "doxygen.h"
#include "outputgen.h"
#include "parserintf.h"

class Doxyparse : public CodeOutputInterface
{
  public:
    Doxyparse(FileDef *fd) : m_fd(fd) {}
   ~Doxyparse() {}

    // these are just null functions, they can be used to produce a syntax highlighted
    // and cross-linked version of the source code, but who needs that anyway ;-)
    void codify(const char *) {}
    void writeCodeLink(const char *,const char *,const char *,const char *,const char *)  {}
    void startCodeLine() {}
    void endCodeLine() {}
    void startCodeAnchor(const char *) {}
    void endCodeAnchor() {}
    void startFontClass(const char *) {}
    void endFontClass() {}
    void writeCodeAnchor(const char *) {}
    void writeLineNumber(const char *,const char *,const char *,int) {}

    void linkableSymbol(int l, const char *sym, Definition *symDef, Definition *context)
    {
      if (!symDef) {
        // in this case we have a local or external symbol

        // TODO record use of external symbols
        // TODO must have a way to differentiate external symbols from local variables
      }
    }

  private:
    FileDef *m_fd;
};

static void findXRefSymbols(FileDef *fd)
{
  // get the interface to a parser that matches the file extension
  ParserInterface *pIntf=Doxygen::parserManager->getParser(fd->getDefFileExtension());

  // reset the parsers state
  pIntf->resetCodeParserState();

  // create a new backend object 
  Doxyparse *parse = new Doxyparse(fd);

  // parse the source code
  pIntf->parseCode(*parse, 0, fileToString(fd->absFilePath()), FALSE, 0, fd);

  // dismiss the object.
  delete parse;
}

static bool ignoreStaticExternalCall(MemberDef *context, MemberDef *md) {
  if (md->isStatic()) {
    if(md->getFileDef()) {
      if(md->getFileDef()->getFileBase() == context->getFileDef()->getFileBase())
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

static void printReferencesMembers(MemberDef *md) {
  LockingPtr<MemberSDict> defDict = md->getReferencesMembers();
  if (defDict != 0) {
    MemberSDict::Iterator msdi(*defDict);
    MemberDef *rmd;
    for (msdi.toFirst(); (rmd=msdi.current()); ++msdi) {
      if (rmd->definitionType() == Definition::TypeMember && !ignoreStaticExternalCall(md, rmd)) {
        printf("      uses %s %s", rmd->memberTypeName().data(), rmd->name().data());
        if (rmd->getFileDef())
          printf(" defined in %s\n", rmd->getFileDef()->getFileBase().data());
        else
          printf("\n");
      }
    }
  }
}

static void lookupSymbol(Definition *d) {
  if (d->definitionType() == Definition::TypeMember) {
    MemberDef *md = (MemberDef *)d;
      printf("   %s %s in line %d\n", md->memberTypeName().data(), d->name().data(), d->getDefLine());
      if (md->isFunction())
        printReferencesMembers(md);
  }
}

static void listSymbols() {
  // iterate over the input files
  FileNameListIterator fnli(*Doxygen::inputNameList); 
  FileName *fn;
  // for each file with a certain name
  for (fnli.toFirst(); (fn=fnli.current()); ++fnli) {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (; (fd=fni.current()); ++fni) {
      MemberList *ml = fd->getMemberList(MemberList::allMembersList);
      if (ml && ml->count() > 0) {
        printf("module %s\n", fd->getFileBase().data());
        MemberListIterator mli(*ml);
        MemberDef *md;
        for (mli.toFirst(); (md=mli.current()); ++mli) {
          lookupSymbol((Definition*) md);
        }
        printf("\n");
      }
    }
  }
  // TODO print external symbols referenced
}

int main(int argc,char **argv) {
  if (argc < 2) {
    printf("Usage: %s [source_file | source_dir]\n",argv[0]);
    exit(1);
  }

  // initialize data structures 
  initDoxygen();

  // setup the non-default configuration options

  // we need a place to put intermediate files
  Config_getString("OUTPUT_DIRECTORY")="/tmp/doxyparse"; 
  // enable HTML (fake) output to omit warning about missing output format
  Config_getBool("GENERATE_HTML")=TRUE;
  // disable latex output
  Config_getBool("GENERATE_LATEX")=FALSE;
  // be quiet
  Config_getBool("QUIET")=TRUE;
  // turn off warnings
  Config_getBool("WARNINGS")=FALSE;
  Config_getBool("WARN_IF_UNDOCUMENTED")=FALSE;
  Config_getBool("WARN_IF_DOC_ERROR")=FALSE;
  // Extract as much as possible
  Config_getBool("EXTRACT_ALL")=TRUE;
  Config_getBool("EXTRACT_STATIC")=TRUE;
  Config_getBool("EXTRACT_PRIVATE")=TRUE;
  Config_getBool("EXTRACT_LOCAL_METHODS")=TRUE;
  // Extract source browse information, needed 
  // to make doxygen gather the cross reference info
  Config_getBool("SOURCE_BROWSER")=TRUE;
  // find functions call between modules
  Config_getBool("CALL_GRAPH")=TRUE;
  // loop recursive over input files
  Config_getBool("RECURSIVE")=TRUE;
  // set the input
  Config_getList("INPUT").clear();
  for (int i = 1; i < argc; i++) {
    Config_getList("INPUT").append(argv[i]);
  }

  // check and finalize the configuration
  checkConfiguration();
  adjustConfiguration();

  // parse the files
  parseInput();

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

  // remove temporary files
  if (!Doxygen::objDBFileName.isEmpty()) unlink(Doxygen::objDBFileName);
  if (!Doxygen::entryDBFileName.isEmpty()) unlink(Doxygen::entryDBFileName);
  // clean up after us
  rmdir(Config_getString("OUTPUT_DIRECTORY"));

  listSymbols();

  exit(0);
}
