#include <doxyparse.h>
#include "doxygen.h"
#include "message.h"

static StringList g_inputFiles;         

int main(int argc, char** argv) {
  initDoxygen();
  readConfiguration(argc,argv);
  checkConfiguration();
  adjustConfiguration();
  searchInputFiles(g_inputFiles);
  QCString *s=g_inputFiles.first();
  while (s) {
    QCString fileName=*s;
    msg("Parsing file %s...\n", fileName.data());
    // ... parse file!
    s=g_inputFiles.next();
  }
  return 0;
}
