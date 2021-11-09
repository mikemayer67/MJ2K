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

#ifndef _XFRM_QCD_H_
#define _XFRM_QCD_H_

#include "transformer.h"

#include <sstream>

//----------------------------------------
// Table A-28 Sqcd (and Sqcc)
//----------------------------------------
// xxx00000            : no quantication                spqcd = 8bits/Table A-29
// xxx00001            : scalar derived (Equation E.5)  spqcd = 16bits/Table A-30
// xxx00010            : scalar expounded               spqcd = 16bits/Table A-30
// 000xxxxx - 111xxxxx : number of guard bits
//----------------------------------------

class Xfrm_Sqcd : public Transformer
{
  public:
    bool wide(void) const { return true; }

    std::string operator()(uint32_t value) const
    {
      uint8_t type = (value & 0x1f);
      uint8_t bits = (value & 0xe0) >> 5;

      std::stringstream rval;
      switch(type) {
        case 0:
          rval << "no quantization";
          break;
        case 1:
          rval << "scalar derived. Use Equation E.5";
          break;
        case 2:
          rval << "scalar expounded";
          break;
        default:
          std::stringstream err;
          err << "Unknown Sqcd value (" << std::hex << uint16_t(type) << ")";
          throw err.str();
          break;
      }

      rval << "  (" << uint16_t(bits) << " guard bits)";
      return rval.str();
    }
};

//----------------------------------------
// Table A-29 SPqcd (and Sqcc)
//----------------------------------------
// 00000xxx - 11111xxx : Exponent epsilon_b Equation E.5
//----------------------------------------
// Table A-30 SPqcd (and Sqcc)
//----------------------------------------
// xxxxx00000000000 - xxxxx11111111111 : mantissa mu_b Equation E.3
// 00000xxxxxxxxxxx - 00000xxxxxxxxxxx : exponent mu_b Equation E.3
//----------------------------------------

class Xfrm_SPqcd : public Transformer
{
  public:

    Xfrm_SPqcd(uint8_t Sqcd)
      : _quantitized( (Sqcd&0x1f)!=0x00 )
    {}

    bool wide(void) const { return false; }

    std::string operator()(uint32_t value) const
    {
      std::stringstream rval;
      if( _quantitized )
      {
        uint16_t mantissa = (value & 0x07ff);
        uint16_t exponent = (value & 0xf800) >> 11;
        rval << "(" << exponent << "," << mantissa << ")";
      }
      else
      {
        uint16_t exponent = (value & 0xf8) >> 3;
        rval << exponent;
      }
      return rval.str();
    }

  private:
    bool _quantitized;
};

#endif // _XFRM_QCD_H_

