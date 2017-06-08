#ifndef DOXYPARSECONFIG_H
#define DOXYPARSECONFIG_H

#include "doxygen.h"
#include "config.h"
#include <string>
#include <sstream>
#include <unistd.h>
#include <cstdlib>


class DoxyparseConfig
{
  public:
    DoxyparseConfig();
   ~DoxyparseConfig();

    const char* getTemporaryDirectoryPath();

    void config();

  private:
    std::ostringstream temporaryDirectory;

    void setDoxyparseConfiguration();
    void removeTemporaryDirectory();
};

#endif
