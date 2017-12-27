#include "doxyparse.h"

Doxyparse::Doxyparse()
{
  initDoxygen();
}

Doxyparse::~Doxyparse()
{
  removeTemporaryFiles();
}

void Doxyparse::configure()
{
  DoxyparseConfig doxyparseConfig;
  doxyparseConfig.config();
}

bool Doxyparse::setInput(int argc, char **argv)
{
  DoxyparseInput doxyparseInput;
  const bool successful_set_the_input = doxyparseInput.set(argc, argv);
  if(successful_set_the_input)
  {
    doxyparseInput.parse();
  }
  return successful_set_the_input;
}

void Doxyparse::parse()
{
  Doxyparser doxyparser;
  doxyparser.parseReferences();
}

void Doxyparse::listResults()
{
  DoxyparseResults doxyparseResults;
  doxyparseResults.listSymbols();
}

void Doxyparse::removeTemporaryFiles()
{
  if(!Doxygen::objDBFileName.isEmpty())
  {
    unlink(Doxygen::objDBFileName);
  }

  if(!Doxygen::entryDBFileName.isEmpty())
  {
    unlink(Doxygen::entryDBFileName);
  }

  rmdir(Config_getString(OUTPUT_DIRECTORY));
}
