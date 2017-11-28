#ifndef DOXYPARSECONFIG_H
#define DOXYPARSECONFIG_H

#include "doxygen.h"
#include "config.h"
#include <sstream>
#include <string>
#include <unistd.h>


class DoxyparseConfig
{
  public:
    DoxyparseConfig();
   ~DoxyparseConfig();

    void config();

  private:
    std::ostringstream temporaryDirectory;

    void set_doxyparse_configuration();
    void remove_temporary_directory();
    const char* get_temporary_directory_path();
};

#endif
