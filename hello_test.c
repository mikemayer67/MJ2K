#include <stdio.h>
#include "hello.h"

int main(int argc,char **argv)
{
  char dst[1024];

  printf("%s\n",hello_world());

  for(int i=0; i<argc; ++i)
  {
    hello(argv[i],dst);
    printf("%s\n",dst);
  }
}

