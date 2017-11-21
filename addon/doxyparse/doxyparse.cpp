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
  DoxyparseConfig doxyparse_config;
  doxyparse_config.config(); 
}

bool Doxyparse::set_input(int argc, char **argv)
{
  DoxyparseInput doxyparse_input;
  const bool successful_set_the_input = doxyparse_input.set(argc, argv);
  if(successful_set_the_input)
  {
    doxyparse_input.parse();
  }
  return successful_set_the_input;
}

void Doxyparse::parse()
{
  Doxyparser doxyparser;
  doxyparser.parse_references();
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
