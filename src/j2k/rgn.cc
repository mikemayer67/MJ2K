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

#include "rgn.h"

#include "formatter.h"

void RGN::init(int fd, const SIZ &siz)
{
  MarkerSegment::init(fd, Marker::RGN );

  _lenC = (siz.Csiz() < 257 ? 1 : 2);

  _C    = get_field(fd, _lenC, "Crgn");
  _S    = get_field(fd, 1,     "Srgn");
  _SP   = get_field(fd, 1,     "SPrgn");
}

void RGN::display(std::ostream &s) const
{
  Formatter out(s);
  out.display(_size,                 2, "Lrgn");
  out.display(_C,                _lenC, "Crgn",  "component index");
  out.display(_S,                    1, "Srgn",  "0=implicit ROI");
  out.display(_SP,                   1, "SPrgn", "binary shift of ROI coefficients");
}
