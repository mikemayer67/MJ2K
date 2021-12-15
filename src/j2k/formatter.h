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
