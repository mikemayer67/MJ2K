#ifndef _XFRM_COM_H_
#define _XFRM_COM_H_

#include "transformer.h"

//----------------------------------------
// Table A-41 Rcom
//----------------------------------------
// 0 : general use (binary values)
// 1 : general use (latin values)
//----------------------------------------

class Xfrm_Rcom : public Transformer
{
  public:
    bool wide(void) const { return false; }

    std::string operator()(uint32_t value) const
    {
      return (value ? "latin" : "binary");
    }
};

#endif // _XFRM_COM_H_
