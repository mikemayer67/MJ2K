#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <stdexcept>

#include "codestream.h"

int main(int argc, char **argv)
{
  if( argc != 2 )
  {
    std::cerr << "Usage:: " << argv[0] << " j2k_filename" << std::endl;
    exit(1);
  }

  std::string filename = argv[1];

  int src = open(argv[1],O_RDONLY);
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
