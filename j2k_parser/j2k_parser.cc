#include <iostream>
#include <fcntl.h>
#include <string.h>

#include "codestream.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char **argv)
{
  if( argc != 2 )
  {
    cerr << "Usage:: " << argv[0] << " j2k_filename" << endl;
    exit(1);
  }

  string filename = argv[1];

  int src = open(argv[1],O_RDONLY);
  if( src < 0 )
  {
    cerr << "Sorry:: Failed to open " << filename << ": " << strerror(errno) << endl;
    exit(1);
  }

  try
  {
    J2KCodestream j2k(src);

    std::cout << j2k;
  }
  catch(string err)
  {
    cerr 
      << endl 
      << "Failed to parse " << filename << ": " << endl
      << err << endl
      << endl;
    exit(1);
  }
  catch(const char *err)
  {
    cerr 
      << endl 
      << "Failed to parse " << filename << ": " << endl
      << err << endl
      << endl;
    exit(1);
  }

  cout << endl << "DONE" << endl << endl;

  return 0;
}
