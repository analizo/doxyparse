#include "doxyparseconfig.h"

DoxyparseConfig::DoxyparseConfig()
{
  temporaryDirectory << "/tmp/doxyparse-" << getpid();
}

DoxyparseConfig::~DoxyparseConfig()
{
  remove_temporary_directory();
}

void DoxyparseConfig::config()
{
  checkConfiguration(); // doxygen method
  this->set_doxyparse_configuration();
  adjustConfiguration();
}

void DoxyparseConfig::set_doxyparse_configuration()
{
  // We need special chars as they are
  Config_getBool(MODIFY_SPECIAL_CHARS)=FALSE;

  Config_getString(OUTPUT_DIRECTORY)= get_temporary_directory_path();

  // enable HTML (fake) output to omit warning about missing output format
  Config_getBool(GENERATE_HTML)=TRUE;

  Config_getBool(GENERATE_LATEX)=FALSE;

  Config_getBool(QUIET)=TRUE;

  Config_getBool(WARNINGS)=FALSE;

  Config_getBool(WARN_IF_UNDOCUMENTED)=FALSE;

  Config_getBool(WARN_IF_DOC_ERROR)=FALSE;

  // Extract as much as possible
  Config_getBool(EXTRACT_ALL)=TRUE;
  Config_getBool(EXTRACT_STATIC)=TRUE;
  Config_getBool(EXTRACT_PRIVATE)=TRUE;
  Config_getBool(EXTRACT_LOCAL_METHODS)=TRUE;

  // Extract source browse information, needed
  // to make doxygen gather the cross reference info
  Config_getBool(SOURCE_BROWSER)=TRUE;

  // find functions call between modules
  Config_getBool(CALL_GRAPH)=TRUE;

  // loop recursive over input files
  Config_getBool(RECURSIVE)=TRUE;
}

const char* DoxyparseConfig::get_temporary_directory_path()
{
  return temporaryDirectory.str().c_str();
}

void DoxyparseConfig::remove_temporary_directory()
{
  std::string cleanup_command = "rm -rf ";
  cleanup_command += temporaryDirectory.str();
  system(cleanup_command.c_str());
}
