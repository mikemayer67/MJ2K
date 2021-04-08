#ifndef _XFRM_CRG_H_
#define _XFRM_CRG_H_

#include "transformer.h"

#include <sstream>

class Xfrm_Xcrg : public Transformer
{
  public:
    bool wide(void) const { return false; }

    std::string operator()(uint32_t value) const
    {
      std::stringstream rval;
      rval << double(value)/65536;
      return rval.str();
    }
};

#endif // _XFRM_CRG_H_

