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

#ifndef _SOT_H_
#define _SOT_H_

#include "segment.h"

//------------------------------------------------------------------------------
// SOT - Start of tile-part
//   Fixed info marker
//   File Header: not allowed
//   Tile Header: required (first marker in tile-part)
//------------------------------------------------------------------------------

class SOT : public MarkerSegment
{
  public:
    SOT(void)   {}
    SOT(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t Lsot  (void) const { return _size; }
    uint16_t Isot  (void) const { return _I;    } 
    uint32_t Psot  (void) const { return _P;    } 
    uint8_t  TPsot (void) const { return _TP;   } 
    uint8_t  TNsot (void) const { return _TN;   }

    uint64_t end_of_data(void) const { return _marker.offset() + _P; }

  private:
    uint16_t _I;
    uint32_t _P;
    uint8_t  _TP;
    uint8_t  _TN;
};


#endif // _SOT_H_
