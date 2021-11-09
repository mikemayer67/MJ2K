#ifndef _TRANSFORMER_H_
#define _TRANSFORMER_H_

#include <string>
#include <stdint.h>

class Transformer
{
  public:    
    virtual std::string operator()(uint32_t value) const 
    { 
      return std::to_string(value); 
    }

    virtual bool wide(void) const 
    { 
      return false;          
    }
};


#endif // _TRANSFORMER_H_
