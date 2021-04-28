#ifndef _HELLO_H
#define _HELLO_H

#include <stdint.h>

typedef struct
{
  int length;
  int ncalls;
  char *last_result;

  union { int  n; char c; } version;

} STATS;

extern void setup_stats();

extern STATS *stats();

extern char *hello(const char *src, char *dst);

extern const char *hello_world(void);

extern uint32_t *gen_data(uint16_t length);

#endif
