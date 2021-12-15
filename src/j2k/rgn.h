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

#ifndef _RGN_H_
#define _RGN_H_

#include "segment.h"
#include "siz.h"

//------------------------------------------------------------------------------
// RGN - Region of Inteest
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

class RGN : public MarkerSegment
{
  public:
    RGN(void)                   {}
    RGN(int fd, const SIZ &siz) { init(fd,siz); }

    void init(int fd, const SIZ &siz);
    void display(std::ostream &s) const;

  public:

    uint16_t Lrgn   (void) const { return _size; }
    uint16_t Crgn   (void) const { return _C;    }
    uint8_t  Srgn   (void) const { return _S;    }
    uint8_t  SPrgn  (void) const { return _SP;   }

  private:
    uint8_t  _lenC;

    uint16_t _C;
    uint8_t  _S;
    uint8_t  _SP;
};

#endif // _RGN_H_ 
