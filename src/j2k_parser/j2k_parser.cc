/*******************************************************************************
 * (C) Copyright 2021, Lockheed Martin Corporation 
 *
 * Government Unlimited Rights
 *
 * This software/technical data and modifications thereof are distributed
 * WITHOUT WARRANTY and is covered by the CPA GSR Agreement of Use. This
 * software/technical data can be used, copied, modified, distributed, or
 * redistributed under the same Agreement. A copy of the Agreement of Use is
 * provided with the software/technical data/modification copy. Access and use
 * may be limited by distribution statements, ITAR regulations, or export
 * control rules. This product is delivered "as is" and without warranty of any
 * kind.
 *******************************************************************************/

#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <stdexcept>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include "codestream.h"

bool gVerbose = false;

void usage(const char *command)
{
  std::cerr << "Usage:: " << command << " [-v|--verbose] j2k_filename" << std::endl;
  exit(1);
}

int main(int argc, char **argv)
{
  std::string filename;

  std::vector<std::string> args;
  for(int i=1; i<argc; ++i) { // skip command arg
    std::string arg = argv[i];
    if( arg == "-v" || arg == "--verbose" ) { gVerbose = true; }
    else if(arg[0] == '-')                  { usage(argv[0]); }
    else if(filename.empty())               { filename = arg; }
    else                                    { usage(argv[0]); }
  }
  if(filename.empty()) { usage(argv[0]); }

  int src = open((char *)(filename.c_str()),O_RDONLY);
  if( src < 0 )
  {
    std::cerr 
      << "Sorry:: Failed to open " << filename << ": " << strerror(errno) << std::endl;
    exit(1);
  }

  try
  {
    J2KCodestream j2k(src);
    std::cout << j2k;
  }
  catch(std::out_of_range e)
  {
    std::cerr << std::endl
      << "Internal error, attempted to index out of range" << std::endl
      << std::endl;
    exit(1);
  }
  catch(std::string err)
  {
    std::cerr << std::endl 
      << "Failed to parse " << filename << ": " << std::endl
      << err << std::endl
      << std::endl;
    exit(1);
  }
  catch(const char *err)
  {
    std::cerr 
      << std::endl << "Failed to parse " << filename << ": " << std::endl
      << err << std::endl
      << std::endl;
    exit(1);
  }

  std::cout << std::endl;

  return 0;
}
