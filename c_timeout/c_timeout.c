#include "to.h"
#include <stdio.h>
#include <unistd.h>

void doit(void *userdata)
{
  int *counter = (int *)userdata;

  for(int i=0; i<20; ++i)
  {
    *counter += 10*i;
    printf("doit:: %d: %d\n",i,*counter);
    sleep(1);
  }
}

int main(int argc,char **argv)
{
  printf("Starting Run With Short Timeout\n");
  int counter = 0;
  int rc = timeout_run(doit, &counter, 10);
  printf("Completed with rc=%d\n\n",rc);

  printf("Starting Run Without Timeout\n");
  doit(&counter);
  printf("Done\n\n");

  printf("Starting Run With Long Timeout\n");
  rc = timeout_run(doit, &counter, 150);
  printf("Completed with rc=%d\n\n",rc);

  return 0;
}
