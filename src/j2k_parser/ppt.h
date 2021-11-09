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

#ifndef _PPT_H_
#define _PPT_H_

#include "segment.h"
#include "rawdata.h"

//------------------------------------------------------------------------------
// PPT - Packed packet headers, tile-part
//   Functional marker
//   File Header: not allowed
//   Tile Header: optional
//------------------------------------------------------------------------------

class PPT : public MarkerSegment
{
  public:
    PPT(void)   {}
    PPT(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t       Lppt(void) const { return _size; }
    uint8_t        Zppt(void) const { return _Z; }
    const RawData &Ippt(void) const { return _I; }

  private:
    uint8_t _Z;
    RawData _I;
};

#endif // _PPT_H_
