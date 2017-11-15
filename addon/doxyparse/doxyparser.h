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

    void parse_references();

  private:
    void find_x_ref_symbols(FileDef *fd);
};

#endif
