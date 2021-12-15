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

