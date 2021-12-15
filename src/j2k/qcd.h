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

#ifndef _QCD_H_
#define _QCD_H_

#include "segment.h"

#include <vector>

//------------------------------------------------------------------------------
// QCD - Quanitization Defaut
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

class QCD : public MarkerSegment
{
  public:
    QCD(void)   {}
    QCD(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t Lqcd  (void) const { return _size; }
    uint8_t  Sqcd  (void) const { return _S; }

    uint16_t SPqcd(uint16_t i) const { return _SP.at(i); }

  private:
    uint8_t               _S;
    std::vector<uint16_t> _SP;
};

#endif // _QCD_H_
