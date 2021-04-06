#ifndef _XFRM_COC_H
#define _XFRM_COC_H

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

#endif // _XFRM_COC_H
