/*******************************************************************************
 * (C) Copyright 2021, Lockheed Martin Corporation 
 *
 * Government Unlimited Rights
 *
 * This software/technical data and modifications thereof are distributed
 * WITHOUT WARRANTY and is covered by the CPA GSR Agreement of Use. This
 * software/technical data can be used, copied, modified, distributed, or
 * redistributed under the same Agreement. A copy of the Agreement of Use is
 * provided with the software/technical data/modification copy. Access and use
 * may be limited by distribution statements, ITAR regulations, or export
 * control rules. This product is delivered "as is" and without warranty of any
 * kind.
 *******************************************************************************/

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
    throw err.str();
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

#if __BYTE_ORDER ==  __LITTLE_ENDIAN
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

#if __BYTE_ORDER == __LITTLE_ENDIAN
    tmp = byte;
    rval |= ( tmp << 8*(nbytes-i-1) );
#else
    rval <<= 8;
    rval |=  byte;
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

