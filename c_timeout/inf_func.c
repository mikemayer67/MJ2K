#include "inf_func.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

int inf_func(void)
{
  uint64_t counter = 0;
  while(1) {
    sleep(1);
    ++counter;
    printf("Counter = %llu\n", counter);
  }
  return 0;

}


