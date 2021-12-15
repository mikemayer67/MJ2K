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

#ifndef _QCC_H_
#define _QCC_H_

#include "segment.h"
#include "siz.h"

#include <vector>

//------------------------------------------------------------------------------
// QCC - Quanitization Component
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

class QCC : public MarkerSegment
{
  public:
    QCC()                       {}
    QCC(int fd, const SIZ &siz) { init(fd,siz); }

    void init(int fd,const SIZ &siz);
    void display(std::ostream &s) const;

    uint16_t Lqcc  (void) const { return _size; }
    uint16_t Cqcc  (void) const { return _C; }
    uint8_t  Sqcc  (void) const { return _S; }

    uint16_t SPqcc(uint16_t i) const { return _SP.at(i); }

  private:
    uint8_t               _lenC;
    uint8_t               _lenSP;

    uint16_t              _C;
    uint8_t               _S;
    std::vector<uint16_t> _SP;
};

#endif // _QCC_H_
