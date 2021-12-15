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

#ifndef _CODESTREAM_H_
#define _CODESTREAM_H_

#include "marker.h"
#include "fileheader.h"
#include "tilepart.h"

#include <iostream>
#include <vector>

class J2KCodestream
{
  public:
    typedef std::vector<J2KTilePart> TileParts_t;

  public:
    J2KCodestream(int fd);

    void display(std::ostream &s) const;

  private:
    J2KFileHeader _header;
    TileParts_t   _tiles;
    Marker        _eoc;
};

static std::ostream &operator<<(std::ostream &s,const J2KCodestream &x)
{
  x.display(s);
  return s;
}

#endif // _CODESTREAM_H_
