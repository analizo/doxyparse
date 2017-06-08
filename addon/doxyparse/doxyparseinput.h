#ifndef DOXYPARSEINPUT_H
#define DOXYPARSEINPUT_H

#include "doxygen.h"
#include "config.h"

class DoxyparseInput
{
  public:
    DoxyparseInput();
   ~DoxyparseInput();

    bool set(int argc, char **argv);
    void parse();
};

#endif
