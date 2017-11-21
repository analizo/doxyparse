/******************************************************************************
 *
 * Copyright (C) 2009 by Joenio Costa.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

/** @file
 *  @brief Code parse based on doxyapp by Dimitri van Heesch
 *
 */

#include "doxyparse.cpp"

int main(int argc,char **argv)
{

  if (argc < 2)
  {
    printf("Usage: %s [source_file | source_dir]\n",argv[0]);
    exit(1);
  }

  Doxyparse doxyparse;

  doxyparse.configure(); 

  if(!doxyparse.set_input(argc, argv))
  {
    exit(1);
  }

  doxyparse.parse();

  doxyparse.listResults();
  exit(0);
}
