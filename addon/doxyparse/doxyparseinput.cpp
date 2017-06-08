#include "doxyparseinput.h"

DoxyparseInput::DoxyparseInput(){}
DoxyparseInput::~DoxyparseInput(){}

bool DoxyparseInput::set(int argc, char **argv)
{
  bool successful_set_the_input = true;
  Config_getList(INPUT).clear();
  for(int i = 1; i < argc; i++)
  {
    if(strcmp(argv[i], "-") == 0)
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
    else
    {
      Config_getList(INPUT).append(argv[i]);
    }
  }
  if(Config_getList(INPUT).isEmpty())
  {
    successful_set_the_input = false;
  }
  return successful_set_the_input;
}

void DoxyparseInput::parse()
{
  parseInput(); // Doxygen
}
