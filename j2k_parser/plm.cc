#include "plm.h"
#include "formatter.h"

#include <unistd.h>

void PLM::init(int fd)
{
  MarkerSegment::init(fd, Marker::PLM );

  _Z    = get_field(fd, 1, "Zplm");

  uint32_t sod = lseek(fd,0,SEEK_CUR);
  uint32_t eod = sod + (_size-3);
  uint32_t cur = sod;

  while( cur < eod )
  {
    PacketLengths_t pl;

    pl.Nplm = get_field(fd,4,"Nplm");
    cur += 1;
    if(cur + pl.Nplm < eod)
    {
      pl.Iplm.read(fd,pl.Nplm);
      cur += pl.Nplm;
    }

    _data.push_back(pl);
  }
}

void PLM::display(std::ostream &s) const
{
  Formatter out(s);
  out.display(_size, 2, "Lplm");
  out.display(_Z,    1, "Zplm", "PLM index");

  for(uint16_t i=0; i<_data.size(); ++i)
  {
    const PacketLengths_t &pl = _data.at(i);
    out.display(pl.Nplm, 1, "Nplm", i, "Iplm length (bytes)");

    s << pl.Iplm;
  }
}

