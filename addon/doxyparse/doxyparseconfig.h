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

    void setDoxyparseConfiguration();
    void removeTemporaryDirectory();
    const char* getTemporaryDirectoryPath();
};

#endif
