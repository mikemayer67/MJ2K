#include "crg.h"
#include "formatter.h"
#include "xfrm_crg.h"

void CRG::init(int fd)
{
  MarkerSegment::init(fd, Marker::CRG );

  uint16_t nrec = (_size - 2)/4;

  for(int i=0; i<nrec; ++i)
  {
    Data_t xy;
    xy.X = get_field(fd, 2, "Xcrg");
    xy.X = get_field(fd, 2, "Ycrg");
    _data.push_back(xy);
  }
}


void CRG::display(std::ostream &s) const
{
  Xfrm_Xcrg xcrg_xfrm;

  Formatter out(s);
  out.display(_size, 2, "Lcrg");

  for(uint16_t i=0; i<_data.size(); ++i)
  {
    out.display(_data.at(i).X, 2, "Xcrg", i, xcrg_xfrm, "horizontal offset (XRsiz/65536)");
    out.display(_data.at(i).Y, 2, "Ycrg", i, xcrg_xfrm, " vertical  offset (YRsiz/65536)");
  }
}

