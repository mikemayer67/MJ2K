#include "ppm.h"
#include "formatter.h"

#include <unistd.h>

void PPM::init(int fd)
{
  MarkerSegment::init(fd, Marker::PPM );

  _Z    = get_field(fd, 1, "Zppm");

  uint32_t sod = lseek(fd,0,SEEK_CUR);
  uint32_t eod = sod + (_size-3);
  uint32_t cur = sod;

  while( cur < eod )
  {
    PacketHeaders_t ph;

    ph.Nppm = get_field(fd,4,"Nppm");
    cur += 4;
    if(cur + ph.Nppm < eod)
    {
      ph.Ippm.read(fd,ph.Nppm);
      cur += ph.Nppm;
    }

    _data.push_back(ph);
  }
}

void PPM::display(std::ostream &s) const
{
  Formatter out(s);
  out.display(_size, 2, "Lppm");
  out.display(_Z,    1, "Zppm", "PPM index");

  for(uint16_t i=0; i<_data.size(); ++i)
  {
    const PacketHeaders_t &ph = _data.at(i);
    out.display(ph.Nppm, 4, "Nppm", i, "Ippm length (bytes)");

    s << ph.Ippm;
  }
}

