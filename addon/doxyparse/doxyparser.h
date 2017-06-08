#ifndef DOXYPARSER_H
#define DOXYPARSER_H

#include <unistd.h>
#include "doxygen.h"
#include "outputgen.h"
#include "parserintf.h"
#include "classlist.h"
#include "filedef.h"
#include "util.h"
#include "filename.h"
#include "arguments.h"
#include "memberlist.h"
#include "types.h"
#include "doxyparseinterface.h"
#include <cstdlib>

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
