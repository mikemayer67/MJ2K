#ifndef _FORMATTER_H_
#define _FORMATTER_H_

#include "transformer.h"

#include <string>

class Formatter
{
  public:
    Formatter(std::ostream &s) : _s(s) {}

    void display(
        uint32_t     value,
        int          nbytes,
        std::string  key,
        int          index,
        Transformer &xform, 
        std::string  info="");

    void display(
        uint32_t     value,
        int          nbytes,
        std::string  key,
        Transformer &xform, 
        std::string  info="")
    { 
      display(value, nbytes, key, -1, xform, info); 
    }

    void display(
        uint32_t     value,
        int          nbytes,
        std::string  key,
        std::string  info="" ) 
    { 
      display(value, nbytes, key, -1, default_xform, info); 
    }

    void display(
        uint32_t     value,
        int          nbytes,
        std::string  key,
        int          index,
        std::string  info="")
    {
      display(value, nbytes, key, index, default_xform, info); 
    }

  static Transformer default_xform;

  private:
    std::ostream &_s;

};


#endif // _FORMATTER_H_
