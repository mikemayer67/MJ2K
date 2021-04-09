#include "marker.h"
#include "endian.h"
 
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <stdint.h>

// ***IMPORTANT NOTE***
//   If advance is false, this is a look ahead operation (file pointer is not advanced)

Marker::Marker(int fd,bool advance)
{
  _read(fd);
  if(!advance) lseek(fd,-2,SEEK_CUR);
}

Marker::Marker(int fd, Marker::Code_t expected)
{
  _read(fd);

  if( _code != expected )
  {
    std::stringstream err;
    err << "Expected to find " << code_to_str(expected)
      << " marker, but found " << code_to_str(_code);
    throw err.str();
  }
}

void Marker::_read(int fd)
{
  _offset = lseek(fd,0,SEEK_CUR);

  uint16_t code(0);
  int nread = read(fd, &code, 2);
  if(nread != 2) 
  {
    std::cout << nread << std::endl;
    std::cout << lseek(fd, 0, SEEK_CUR) << std::endl;
    throw "Failed to read marker code";
  }

#ifdef __LITTLE_ENDIAN__
  code = ((code & 0x00ff)<<8) | ((code & 0xff00)>>8);
#endif

  _code = static_cast<Code_t>(code);
}

std::string Marker::code_to_str(Marker::Code_t code)
{
  std::string rval;
  switch(code)
  {
    case SOC: rval = "SOC"; break; 
    case SOT: rval = "SOT"; break; 
    case SOD: rval = "SOD"; break; 
    case EOC: rval = "EOC"; break; 
    case SIZ: rval = "SIZ"; break; 
    case COD: rval = "COD"; break; 
    case COC: rval = "COC"; break; 
    case QCD: rval = "QCD"; break; 
    case QCC: rval = "QCC"; break; 
    case RGN: rval = "RGN"; break; 
    case POC: rval = "POC"; break; 
    case TLM: rval = "TLM"; break; 
    case PLM: rval = "PLM"; break; 
    case PLT: rval = "PLT"; break; 
    case PPM: rval = "PPM"; break; 
    case PPT: rval = "PPT"; break; 
    case SOP: rval = "SOP"; break; 
    case EPH: rval = "EPH"; break; 
    case CRG: rval = "CRG"; break; 
    case COM: rval = "COM"; break; 
    default:  rval = "???"; break;

  }
  return rval;
}

void Marker::display(std::ostream &s) const
{
  s << str() << ": [0x" 
    << std::hex << std::setfill('0') << std::setw(8) << _offset
    << std::setfill(' ') << std::dec
    << "]" << std::endl;
}
