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

#include "codestream.h"

#include "marker.h"

#include <sstream>
#include <unistd.h>

extern bool gVerbose;

J2KCodestream::J2KCodestream(int fd)
{
  _header.init(fd);

  bool done = false;
  while(!done)
  {
    Marker marker(fd);
    if(gVerbose) { std::cout << marker << std::endl; }
    switch(marker.code())
    {
      case Marker::SOT:
        _tiles.push_back(J2KTilePart(fd, _header.siz() ));
        if(gVerbose) { std::cout << _tiles.back() << std::endl; }
        break;

      case Marker::EOC:
        done = true;
        break;

      default:
        std::stringstream err;
        err << "Next marker must be SOT or EOC, not " << marker.str();
        throw err.str();
        break;
    }
  }

  // Final marker must be EOC
  _eoc = Marker(fd, Marker::EOC);

  uint64_t pos = lseek(fd,0,SEEK_CUR);
  uint64_t eof = lseek(fd,0,SEEK_END);
  if( pos != eof) throw "EOC encountered before end of file";
  lseek(fd,pos,SEEK_SET);
}

void J2KCodestream::display(std::ostream &s) const
{
  s << std::endl
    << "=========== Start of Header ===========" << std::endl
    << std::endl;

  s << _header;

  for(uint16_t i=0; i<_tiles.size(); ++i)
  {
    s << std::endl
      << "=========== File Part " << i+1 << " ===========" << std::endl
      << std::endl;
    s << _tiles.at(i);
  }
  
  s << std::endl
    << "=========== End of File ===========" << std::endl
    << std::endl;

  s << _eoc << std::endl;
}
