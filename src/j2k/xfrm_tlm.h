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

#ifndef _XFRM_TLM_H_
#define _XFRM_TLM_H_

#include "transformer.h"

#include <sstream>

//----------------------------------------
// Table A-34 Stlm
//----------------------------------------
// xx00xxxx : ST = 0, Ttlm is  0 bits
// xx01xxxx : ST = 1, Ttlm is  8 bits
// xx10xxxx : ST = 2, Ttlm is 16 bits
// x0xxxxxx : SP = 0, Ptlm is 16 bits
// x1xxxxxx : SP = 1, Ptlm is 32 bits
//----------------------------------------

class Xfrm_Stlm : public Transformer
{
  public:
    bool wide(void) const { return true; }

    std::string operator()(uint32_t value) const
    {
      uint8_t ST = (value & 0x30) >> 4;
      uint8_t SP = (value & 0x40) >> 6;

      std::stringstream rval;
      rval << "ST=" << uint16_t(ST) << " " 
        << ( ST == 0 ? "No Ttlm" : ST == 1 ? "Ttlm is 8 bits" : "Ttlm is 16 bits")
        << " / SP=" << uint16_t(SP) << " Ptlm is " << uint16_t(16 + 16*SP) << " bits";

      return rval.str();
    }
};


#endif // _XFRM_TLM_H_
