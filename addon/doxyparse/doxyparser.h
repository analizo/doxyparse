#ifndef DOXYPARSER_H
#define DOXYPARSER_H

#include "doxygen.h"
#include "doxyparseinterface.h"
#include "filedef.h"
#include "filename.h"
#include "parserintf.h"
#include "util.h"

class Doxyparser
{
  public:
    Doxyparser();
   ~Doxyparser();

    void parseReferences();

  private:
    void findXRefSymbols(FileDef *fd);
};

#endif
