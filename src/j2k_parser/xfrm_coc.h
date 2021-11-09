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

#ifndef _XFRM_COC_H_
#define _XFRM_COC_H_

#include "transformer.h"
#include "xfrm_cod.h"

#include <sstream>

//----------------------------------------
// Table A-23 Scoc
//----------------------------------------
// xxxxxxx0 : Entropy coder, precincts with PPx=15 and PPy=15
// xxxxxxx1 : Entropy coder, with precincts defined below
//----------------------------------------

class Xfrm_Scoc : public Transformer
{
  public:
    bool wide(void) const { return true; }

    std::string operator()(uint32_t value) const
    {
      std::stringstream rval;
      rval << "Entry coder, precints" << ((value & 0x01) ? " defined below" : "=(15,15)");
      return rval.str();
    }
};

#endif // _XFRM_COC_H_
