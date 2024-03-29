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
