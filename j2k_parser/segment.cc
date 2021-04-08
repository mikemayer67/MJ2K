#include "segment.h"
#include "endian.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unistd.h>

void MarkerSegment::init(int fd, Marker::Code_t code)
{
  _marker = Marker(fd,true);  // consume the marker

  if( ! _marker.is(code) )
  {
    std::stringstream err;
    err << "Expected to find " << Marker(code).str() << " marker, but found " << _marker.str();
    throw err;
  }

  std::string key = std::string("L") + _marker.str();
  std::transform(key.begin()+1, key.end(), key.begin()+1, (int(*)(int))std::tolower);

  _size = get_field(fd, 2, key);
}

void MarkerSegment::head(std::ostream &s) const
{
  _marker.display(s);
}

uint32_t MarkerSegment::
get_field(int fd, int nbytes,std::string field) const
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
      std::stringstream err;
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
throw_range_error(std::string field, uint16_t index, uint16_t array_size) const
{
  std::stringstream err;
  err << "Invalid index (" << index << ") for " << field
    << " (limit= " << array_size-1 << ")";
  throw err.str();
}

