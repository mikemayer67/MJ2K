#include <stdio.h>
#include "hello.h"

#ifndef N
#define N 128
#endif

int main(int argc,char **argv)
{
  char dst[1024];

  printf("%s\n",hello_world());

  for(int i=0; i<argc; ++i)
  {
    hello(argv[i],dst);
    printf("%s\n",dst);
  }

  uint32_t *numbers = (uint32_t *)gen_data(N * sizeof(uint32_t));

  for(int i=0; i<N; ++i)
  {
    printf("%3d: %016x\n", i, numbers[i]);
  }


}

