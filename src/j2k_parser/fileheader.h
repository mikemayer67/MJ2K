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

#ifndef _FILEHEADER_H_
#define _FILEHEADER_H_

#include "marker.h"
#include "siz.h"

#include <iostream>

class J2KFileHeader
{
  public:
    J2KFileHeader(void)   {}
    J2KFileHeader(int fd) { init(fd); }

    void init(int fd);

    void display(std::ostream &s) const;

    const SIZ &siz(void) const { return _siz; }

  private:
    Marker _soc;
    SIZ    _siz;

    std::vector<MarkerSegment*> _segments;
};

static std::ostream &operator<<(std::ostream &s, const J2KFileHeader &x) 
{ 
  x.display(s); 
  return s; 
}

#endif // _FILEHEADER_H_

