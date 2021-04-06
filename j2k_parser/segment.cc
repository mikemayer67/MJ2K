#include "segment.h"
#include "endian.h"

#include <sstream>
#include <unistd.h>

using std::string;
using std::stringstream;

uint32_t MarkerSegment::
get_field(int fd, int nbytes,string field) const
{
  uint8_t byte;
  uint32_t rval(0);

#ifdef __LITTLE_ENDIAN__
  uint32_t tmp(0);
#endif

  for(int i=0; i<nbytes; ++i)
  {
    int nread = read(fd, &byte, 1);
    if( nread != 1 )
    {
      stringstream err;
      err << "Failed to read data for " << field << " field (file offset: "
        << lseek(fd,0,SEEK_CUR) << ")";
      throw err.str();
    }

#ifdef __BIG_ENDIAN__
    rval <<= 8;
    rval |=  byte;
#else
    tmp = byte;
    rval |= ( tmp << 8*(nbytes-i-1) );
#endif
  }

  return rval;
}

void MarkerSegment::
throw_range_error(string field, uint16_t index, uint16_t array_size) const
{
  stringstream err;
  err << "Invalid index (" << index << ") for " << field
    << " (limit= " << array_size-1 << ")";
  throw err.str();
}

