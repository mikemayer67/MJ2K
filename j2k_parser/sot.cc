#include "sot.h"
#include "formatter.h"

void SOT::init(int fd)
{
  MarkerSegment::init(fd, Marker::SOT );

  _I  = get_field(fd, 2, "Isot" );
  _P  = get_field(fd, 4, "Psot" );
  _TP = get_field(fd, 1, "TPsot" );
  _TN = get_field(fd, 1, "TNsot" );
}

void SOT::display(std::ostream &s) const
{
  Formatter out(s);
  out.display(_size,  2, "Lsot");
  out.display(_I,     2, "Isot",  "tile index");
  out.display(_P,     4, "Psot",  "length of tile-part");
  out.display(_TP,    1, "TPsot", "tile part index");
  out.display(_TN,    1, "TNsot", "number of tile parts");
}
