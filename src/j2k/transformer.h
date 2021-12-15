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

#ifndef _TRANSFORMER_H_
#define _TRANSFORMER_H_

#include <string>
#include <sstream>
#include <stdint.h>

class Transformer
{
  public:    
    virtual std::string operator()(uint32_t value) const 
    { 
      std::stringstream s;
      s << value; 
      return s.str();
    }

    virtual bool wide(void) const 
    { 
      return false;          
    }
};


#endif // _TRANSFORMER_H_
