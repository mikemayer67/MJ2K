#include "accumulator.h"

uint32_t accumulate(uint8_t n, uint32_t *data)
{
  uint32_t rval = 0;
  for(uint8_t i=0; i<n; ++i) rval += data[i];
  return rval;
}
