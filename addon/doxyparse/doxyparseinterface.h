#ifndef DOXYPARSEINTERFACE_H
#define DOXYPARSEINTERFACE_H

#include "filedef.h"
#include "outputgen.h"

class DoxyparseInterface : public CodeOutputInterface
{
  public:
    DoxyparseInterface(FileDef *fd) : m_fd(fd) {}
   ~DoxyparseInterface() {}

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
    virtual void writeTooltip(const char *,const DocLinkInfo &,
                              const char *,const char *,const SourceLinkInfo &,
                              const SourceLinkInfo &) {}
    void startCodeLine(bool) {}
    void setCurrentDoc(Definition *,const char *,bool) {}
    void addWord(const char *,bool) {}

    void linkable_symbol(int l, const char *sym, Definition *symDef, Definition *context);

  private:
    FileDef *m_fd;
};

#endif
