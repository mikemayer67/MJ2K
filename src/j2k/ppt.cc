#include "ppt.h"
#include "formatter.h"

void PPT::init(int fd)
{
  MarkerSegment::init(fd, Marker::PPT );

  _Z = get_field(fd, 1, "Zppt");

  _I.read(fd, _size-3);
}

void PPT::display(std::ostream &s) const
{
  Formatter out(s);
  out.display(_size, 2, "Lppt");
  out.display(_Z,    1, "Zppt", "PPT index");
  s << _I;
}

