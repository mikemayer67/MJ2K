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

