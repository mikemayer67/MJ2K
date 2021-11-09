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

#ifndef _POC_H_
#define _POC_H_

#include "segment.h"
#include "siz.h"

#include <vector>

//------------------------------------------------------------------------------
// POC - Region of Inteest
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

class POC : public MarkerSegment
{
  public:
    POC(void)                   {}
    POC(int fd, const SIZ &siz) { init(fd,siz); }

    void init(int fd, const SIZ &siz);
    void display(std::ostream &s) const;

  public:

    struct ProgOrderChange_t
    {
      uint8_t  RS;
      uint16_t CS;
      uint16_t LYE;
      uint8_t  RE;
      uint8_t  S;
      uint16_t CE;
      uint8_t  P;
    };

    uint16_t Lrgn   (void) const { return _size; }

    uint8_t  RSpoc  (uint16_t i) const { return _progOrderChange.at(i).RS; }
    uint16_t CSpoc  (uint16_t i) const { return _progOrderChange.at(i).CS; }
    uint16_t LYEpoc (uint16_t i) const { return _progOrderChange.at(i).LYE; }
    uint8_t  REpoc  (uint16_t i) const { return _progOrderChange.at(i).RE; }
    uint16_t CEpoc  (uint16_t i) const { return _progOrderChange.at(i).CE; }
    uint8_t  Ppoc   (uint16_t i) const { return _progOrderChange.at(i).P; }
  private:
    uint8_t  _lenC;

    std::vector<ProgOrderChange_t> _progOrderChange;
};

#endif // _POC_H_ 
