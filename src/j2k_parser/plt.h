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

#ifndef _PLT_H_
#define _PLT_H_

#include "segment.h"
#include "rawdata.h"

#include <vector>

//------------------------------------------------------------------------------
// PLT - Packet Length, tile-part
//   Functional marker
//   File Header: optional
//   Tile Header: not allowed
//------------------------------------------------------------------------------

class PLT : public MarkerSegment
{
  public:
    PLT(void)   {}
    PLT(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t Lplt(void)  const { return _size; }
    uint8_t  Zplt(void)  const { return _Z; }
    uint32_t Iplt(int i) const { return _I.at(i); }

    int      num_packets(void) const { return _I.size(); }
    uint32_t sum_packet_lengths(void) const { return _sum_Iplt; }

  private:
    uint8_t _Z;
    std::vector<uint32_t> _I;
    uint32_t _sum_Iplt;

    RawData _I_raw;
};

#endif // _PLT_H_
