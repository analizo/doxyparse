#ifndef DOXYPARSEINPUT_H
#define DOXYPARSEINPUT_H

#include <unistd.h>
#include "doxygen.h"
#include "outputgen.h"
#include "config.h"
#include "parserintf.h"
#include "classlist.h"
#include "filedef.h"
#include "util.h"
#include "filename.h"
#include "arguments.h"
#include "memberlist.h"
#include "types.h"
#include <cstdlib>

class DoxyparseInput
{
  public:
    DoxyparseInput();
   ~DoxyparseInput();

    bool set(int argc, char **argv);
    void parse();
};

#endif
