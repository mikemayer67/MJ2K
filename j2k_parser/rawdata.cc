#include "rawdata.h"

#include <iostream>
#include <iomanip>

#include <sstream>
#include <unistd.h>
#include <string.h>

void RawData::release(void)
{
  if(_data )
  {
    if( *_shareCounter > 1 )
    {
      *_shareCounter -= 1;
    }
    else
    {
      delete   _shareCounter;
      delete[] _data;
    }
  }

  _data         = NULL;
  _shareCounter = 0;
  _len          = 0;
}


void RawData::clone(const RawData &ref)
{
  if( this == &ref ) { 
    return;
  }

  release();

  _len          = ref._len;
  _data         = ref._data;
  _shareCounter = ref._shareCounter;

  if( _shareCounter != NULL ) *_shareCounter += 1;
}


void RawData::read(int fd, uint32_t nbytes)
{
  release(); // yes, we could check if this is needed, but it's pretty light-weight

  _data = new uint8_t[nbytes];
  
  uint32_t  toread = nbytes;
  uint8_t  *pos = _data;
  while(toread > 0)
  {
    uint32_t nread = ::read(fd, pos, toread);
    if( nread < 0 )
    {
      delete[] _data;
      _data = NULL;

      std::stringstream err;
      err << "Failed to read raw data starting at ("
        << lseek(fd,0,SEEK_CUR) << ": " << strerror(errno);
      throw err.str();
    }

    toread -= nread;
    pos    += nread;
  }

  _len = nbytes;

  _shareCounter = new uint16_t;
  *_shareCounter = 1;
}

void RawData::dump(std::ostream &s) const
{
  uint8_t *pos = _data;
  uint8_t *end = _data + _len;

  uint8_t curbyte  = 0;
  uint8_t curblock = 0;

  const std::string indent = "     ";
  const std::string gap    = "   ";

  s << std::setfill('0') << std::hex 
    << indent << "-----------" << std::endl << indent;

  while(pos < end)
  {
    s << std::setw(2) << uint16_t(*pos++);

    if(++curbyte == 8) {
      if(++curblock == 4) {
        s << std::endl << indent;
      } else {
        s << gap;
      }
      curblock %= 4;
    }
    else 
    { 
      s << " "; 
    }
    curbyte %= 8;
  }
  if(curbyte > 0) { s << std::endl; }

  s << indent << "-----------" << std::setfill(' ') << std::dec << std::endl;
}
