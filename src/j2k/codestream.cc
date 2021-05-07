#include "codestream.h"

#include "marker.h"

#include <sstream>
#include <unistd.h>

J2KCodestream::J2KCodestream(int fd)
{
  _header.init(fd);

  bool done = false;
  while(!done)
  {
    Marker marker(fd);
    switch(marker.code())
    {
      case Marker::SOT:
        _tiles.push_back(J2KTilePart(fd, _header.siz() ));
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
