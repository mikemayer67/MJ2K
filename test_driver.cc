#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

string cmd = "";

void usage(string err="")
{
  if(err.length() > 0) cout << "Sorry:: " << err << endl;
  cout << "Usage:: " << cmd << " test_index [verbosity]" << endl;
  exit(1);
}

int main(int argc,char **argv)
{
  int rc = 0;

  cmd = argv[0];

  if(argc<2) usage("missing test name");
  if(argc>3) usage("Unkonwn extra arguments");

  string name = argv[1];
  if(name == "help" || name == "-h") usage();
  if(name == "list" || name == "-l") usage();

  char *end = NULL;
  int test_index = strtol(argv[1], &end, 10);
  if(*end != '\0') {
    char err[100];
    snprintf(err,99,"test_index must be a number, not '%s'",argv[1]);
    usage(err);
  }

  int verbose = 0;
  if(argc==3)
  {
    string level = argv[2];
    if     (level == "1" || level == "v") { verbose = 1; }
    else if(level == "2" || level == "d") { verbose = 2; }
    else {
      char err[100];
      snprintf(err,99,"verbose be a 1 or 2, not '%s'",argv[2]);
      usage(err);
    }
  }

  switch(test_index)
  {
    case 1:
      if(verbose)
      {
        cout << "Running Test 1" << endl;
        if(verbose>1) {
          cout << endl 
            << "   and here are some silly little details" << endl
            << endl;
        }
      }
      break;

    case 2:
      if(verbose)
      {
        cout << "Running Test 2" << endl;
        if(verbose>1) {
          cout << endl 
            << "   FAIL:: Cannot run with verbose > 1 " << endl
            << endl;

          rc = 1;
        }
      }
      break;

    case 6:
      if(verbose)
      {
        cout << "Running Test 3" << endl;
      }
      cout << endl
        << "   FAIL:: Test 3 always fails with rc=2" << endl
        << endl;
      rc = 2;
      break;

    default:
      cout << "Sorry... " << test_index << " is not a recognized test index" << endl;
      return 1;
      break;
  }



  return rc;
}
