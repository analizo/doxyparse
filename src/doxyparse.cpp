#include <doxyparse.h>
#include "doxygen.h"
#include "message.h"

int main(int argc, char** argv) {
  initDoxygen();
  //readConfiguration(argc,argv);
  //checkConfiguration();
  //adjustConfiguration();
  //searchInputFiles(g_inputFiles);
  //QCString *s=g_inputFiles.first();
  //while (s) {
  //  QCString fileName=*s;
  //  msg("Parsing file %s...\n", fileName.data());
  //  // ... parse file!
  //  s=g_inputFiles.next();
  //}
  //return 0;
}

void searchInputFiles2(StringList &inputFiles) {
  //QStrList &exclPatterns = ""; // TODO pegar da linha de comando
  //StringDict excludeNameDict(1009);
  //excludeNameDict.setAutoDelete(TRUE);
  //msg("Searching for files to process...\n");
  //QDict<void> *killDict = new QDict<void>(10007);
  //int inputSize=0;
  //QStrList &inputList = "src"; // TODO pegar da linha de comando
  //inputFiles.setAutoDelete(TRUE);
  //s=inputList.first();
  //while (s)
  //{
  //  QCString path=s;
  //  uint l = path.length();
  //  // strip trailing slashes
  //  if (path.at(l-1)=='\\' || path.at(l-1)=='/') path=path.left(l-1);

  //  inputSize+=readFileOrDirectory(
  //      path,
  //      Doxygen::inputNameList,
  //      Doxygen::inputNameDict,
  //      &excludeNameDict,
  //      "*.h *.cpp", // TODO pegar da linha de comando
  //      &exclPatterns,
  //      &inputFiles,0,
  //      true,
  //      TRUE,
  //      killDict);
  //  s=inputList.next();
  //}
  //delete killDict;
}
