#include "plt.h"
#include "formatter.h"

void PLT::init(int fd)
{
  MarkerSegment::init(fd, Marker::PLT );

  _Z = get_field(fd, 1, "Zplt");

  _I.read(fd, _size-3);
}

void PLT::display(std::ostream &s) const
{
  Formatter out(s);
  out.display(_size, 2, "Lplt");
  out.display(_Z,    1, "Zplt", "PLT index");

  s << _I;
}

