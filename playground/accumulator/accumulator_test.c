#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "accumulator.h"

int main(int argc,char **argv)
{
  uint32_t values[256];

  srand(getpid());

  uint8_t n = 5 + rand()%10;

  for(uint8_t i=0; i<n; ++i)
  {
    values[i] = rand()%1000;
  }

  uint32_t sum = accumulate(n, values);

  printf("\n");
  for(uint8_t i=0; i<n; ++i)
  {
    if(i>0) printf(" + ");
    printf("%d", values[i]);
  }
  printf(" = %d\n", sum);
}
