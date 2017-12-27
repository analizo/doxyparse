#include "doxyparseinput.h"

DoxyparseInput::DoxyparseInput(){}
DoxyparseInput::~DoxyparseInput(){}

bool DoxyparseInput::set(int argc, char **argv)
{
  Config_getList(INPUT).clear();

  for(int i = 1; i < argc; ++i)
  {
    const bool has_nested_filenames = strcmp(argv[i], "-") == 0;

    if(has_nested_filenames)
    {
      append_through_filenames();
    }
    else
    {
      Config_getList(INPUT).append(argv[i]);
    }
  }

  return !Config_getList(INPUT).isEmpty();
}

void DoxyparseInput::parse()
{
  parseInput();
}

void DoxyparseInput::append_through_filenames()
{
  bool got_end_of_file = false;
  while(!got_end_of_file)
  {
    char filename[1024];
    scanf("%s[^\n]", filename);

    if(feof(stdin))
    {
      got_end_of_file = true;
    }
    else
    {
      Config_getList(INPUT).append(filename);
    }
  }
}
