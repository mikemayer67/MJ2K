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

#ifndef _TILEPART_H_
#define _TILEPART_H_

#include "marker.h"
#include "siz.h"
#include "sot.h"

#include <iostream>
#include <vector>

class J2KTilePart
{
  public:
    J2KTilePart(void)   {};
    J2KTilePart(int fd, const SIZ &siz) { init(fd,siz); }

    void init(int fd, const SIZ &siz);
    void display(std::ostream &s) const;

  private:
    SOT    _sot;
    Marker _sod;

    std::vector<MarkerSegment*> _segments;
};

static std::ostream &operator<<(std::ostream &s, const J2KTilePart &x) 
{ 
  x.display(s); 
  return s; 
}

#endif // _TILEPART_H_
