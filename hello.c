#include <string.h>
#include <stdlib.h>

#include "hello.h"

static STATS gStats = {0,0,NULL};

static uint32_t *gData = NULL;

void setup_stats()
{
  gStats.length = 0;
  gStats.ncalls = 0;
  gStats.last_result = NULL;
  gStats.version.c = 'x';
}

STATS *stats(void)
{
  return &gStats;
}

char *hello(const char *src, char *dst)
{
  gStats.length += strlen(src);
  gStats.ncalls += 1;

  strcpy(dst,"hello ");
  strcat(dst,src);

  free(gStats.last_result);
  gStats.last_result = malloc(strlen(dst) + 1);
  strcpy(gStats.last_result, dst);

  gStats.version.n = 3;

  return dst;
}

const char *hello_world(void)
{
  gStats.ncalls += 1;

  free(gStats.last_result);
  gStats.last_result = NULL;

  gStats.version.c = 'z';

  return "hello, world";
}

uint32_t *gen_data(uint16_t length)
{
  free(gData);
  gData = malloc(length*sizeof(uint32_t));
  if(gData) {
    for(uint16_t i=0; i<length; ++i) { gData[i] = i; }
  }
  return gData;
}
