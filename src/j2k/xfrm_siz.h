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

#ifndef _XFRM_SIZ_H_
#define _XFRM_SIZ_H_

#include "transformer.h"

#include <sstream>

//----------------------------------------
// Table A-11 Ssiz
//----------------------------------------
// x0000000 - x0100101: componenents are value+1: from 1 bith deep through 38 bits deep
// 0xxxxxxx           : components are unsigned values
// 1xxxxxxx           : components are   signed values
//----------------------------------------

class Xfrm_Ssiz : public Transformer
{
  public:
    bool wide(void) const { return true; }

    std::string operator()(uint32_t value) const
    {
      uint8_t bits = value & 0x7f; 
      uint8_t s    = value & 0x80;

      std::stringstream rval;
      rval << "precision (";
      if( s == 0 )
      {
        uint32_t lim = (1L << bits) - 1;
        rval << "0 - " << lim;
      }
      else
      {
        uint32_t lim = (1L << (bits-1));
        rval << "-" << lim << " - " << lim-1;
      }
      rval << ")";

      return rval.str();
    }
};


#endif // _XFRM_SIZ_H_
