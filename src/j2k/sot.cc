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
  out.display(_TN,    1, "TNsot", "number of tile parts (0=not specified)");
}
