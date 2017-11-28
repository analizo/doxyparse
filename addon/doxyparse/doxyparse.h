#ifndef DOXYPARSE_H
#define DOXYPARSE_H

#include "doxygen.h"
#include "doxyparseconfig.h"
#include "doxyparseinput.h"
#include "doxyparser.h"
#include "doxyparseresults.h"

class Doxyparse
{
  public:
    inline Doxyparse();
    inline ~Doxyparse();

    inline void configure();
    inline bool set_input(int argc, char **argv);
    inline void parse();
    inline void listResults();

  private:
    inline void removeTemporaryFiles();
};

#endif
