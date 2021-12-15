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

#include "plt.h"
#include "formatter.h"

void PLT::init(int fd)
{
  MarkerSegment::init(fd, Marker::PLT );

  _Z = get_field(fd, 1, "Zplt");
  _I_raw.read(fd,_size-3);

  _sum_Iplt = 0;

  uint32_t Iplt = 0;
  for(const uint8_t *p=_I_raw.data(); p!=_I_raw.end(); ++p)
  {
    Iplt <<= 7;
    Iplt += (*p) & 0x7f;

    if( (*p & 0x80) == 0x00 )
    {
      _I.push_back(Iplt);
      _sum_Iplt += Iplt;
      Iplt = 0;
    }
  }
}

void PLT::display(std::ostream &s) const
{
  Formatter out(s);
  out.display(_size, 2, "Lplt");
  out.display(_Z,    1, "Zplt", "PLT index");
  for(int i=0; i<_I.size(); ++i) {
    out.display(_I[i], 4, "Iplt", i, "packet length");
  }
  out.display(_sum_Iplt, 4, "Total", "sum of all packet lengths");
}

