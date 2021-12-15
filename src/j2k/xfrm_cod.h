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

#ifndef _XFRM_COD_H_
#define _XFRM_COD_H_

#include "transformer.h"

#include <sstream>

//----------------------------------------
// Table A-13 Scod
//----------------------------------------
// xxxxxxx0 : Entropy coder, precincts with PPx=15 and PPy=15
// xxxxxxx1 : Entropy coder, with precincts defined below
// xxxxxx0x : No SOP marker segments used
// xxxxxx1x : SOP marker segments may be used
// xxxxx0xx : No EPH marker segments used
// xxxxx1xx : EPH marker segments may be used
//----------------------------------------

class Xfrm_Scod : public Transformer
{
  public:
    bool wide(void) const { return true; }

    std::string operator()(uint32_t value) const
    {
      std::stringstream rval;
      rval 
        << "Entry coder, precints" << ((value & 0x01) ? " defined below" : "=(15,15)")
        << ", SOP " << ((value & 0x02) ? "may be used" : "not used")
        << ", EPH " << ((value & 0x04) ? "may be used" : "not used");
      return rval.str();
    }
};

//----------------------------------------
// Table A-16 Progression Order
//----------------------------------------
// 00000000 : layer-resolution level-component-position
// 00000001 : resolution level-layer-component-position
// 00000010 : resolution level-position-component-layer
// 00000011 : position-component-resolution level-layer
// 00000100 : component-position-resolution level-layer
//----------------------------------------

//----------------------------------------
// Table A-17 Multi-component information
//----------------------------------------
// 00000000 : No transform specified
// 00000001 : transform used on components 0, 1, 2 for coding efficiency
//----------------------------------------

class Xfrm_SGcod : public Transformer
{
  public:
    enum Field { ProgOrder, McompXform };

    Xfrm_SGcod(Field field) : _field(field) {}

    bool wide(void) const { 
      switch( _field ) {
        case ProgOrder:  return false; break;
        case McompXform: return true;  break;
      }
      return false;
    }

    std::string operator()(uint32_t value) const
    {
      std::stringstream rval;
      switch( _field ) {
        case ProgOrder: 
          switch(value) {
            case 0: rval << "LRCP"; break;
            case 1: rval << "RLCP"; break;
            case 2: rval << "RPCL"; break;
            case 3: rval << "PCRL"; break;
            case 4: rval << "CPRL"; break;
            default:
              std::stringstream err;
              err << "Unknown progression order: " << value;
              throw err.str();
              break;
          }
          break;

        case McompXform:
          switch(value) {
            case 0: rval << "no multi component transform"; break;
            case 1: rval << "xform used on components 0-2"; break;
            default:
              std::stringstream err;
              err << "Unknown multi component transform: " << value;
              throw err.str();
              break;
          }
          break;
      }
      return rval.str();
    }

  private:
    Field _field;
};

//----------------------------------------
// Table A-18 Codeblock width/height
//----------------------------------------
// xxxx0000 - xxxx1000 : code block width/height exponent - 2
//----------------------------------------

//----------------------------------------
// Table A-19 Code block style
//----------------------------------------
// xxxxxxx0 : no selective arithmetic coding bypass
// xxxxxxx1 : selective arithmetic coding bypass
//----------------------------------------
// xxxxxx0x : no reset of context probabilities
// xxxxxx1x : reset of context probabilities
//----------------------------------------
// xxxxx0xx : no termination on each coding pass
// xxxxx1xx : termination on each coding pass
//----------------------------------------
// xxxx0xxx : no vertically casual context
// xxxx1xxx : vertically casual context
//----------------------------------------
// xxx0xxxx : no predictable termination
// xxx1xxxx : predictable termination
//----------------------------------------
// xx0xxxxx : no segmentation symbols are used
// xx1xxxxx : segmentation symbols are used
//----------------------------------------

//----------------------------------------
// Table A-20 Transformation
//----------------------------------------
// 00000000 : 9-7 irreversible filter
// 00000001 : 5-3 reversible filter
//----------------------------------------

//----------------------------------------
// Table A-21 Precinct Size
//----------------------------------------
// xxxx0000 - xxxx1111 PPx
// 0000xxxx - 1111xxxx PPy
//----------------------------------------

class Xfrm_SPcod : public Transformer
{
  public:
    enum Field {CodeBlockWidth, CodeBlockHeight, CodeBlockStyle, Wavelet, PrecintSize}; 

    Xfrm_SPcod(Field field) : _field(field) {}

    bool wide(void) const { 
      switch( _field ) {
        case CodeBlockWidth:
        case CodeBlockHeight:
        case Wavelet:
        case PrecintSize:
          return false; 
          break;
        case CodeBlockStyle:
          return true;
          break;
      }
      return false;
    }

    std::string operator()(uint32_t value) const
    {
      std::stringstream rval;
      switch( _field ) {
        case CodeBlockWidth:
        case CodeBlockHeight:
          {
            uint32_t exp = 2 + (value & 0x0f);
            if( exp > 10 ) {
              std::stringstream err;
              err << "Max codeblock width/height exponent is 10 (found" << exp <<")";
              throw err.str();
            }
            rval << (1<<exp);
          }
          break;

        case Wavelet:
          rval << ((value & 0x01) == 0 ? "9-7 VL" : "5-3 NL");
          break;

        case PrecintSize:
          {
            uint16_t ppx = (value & 0x0f);
            uint16_t ppy = (value & 0xf0) >> 4;
            rval << "(" << ppx << "," << ppy << ")";
          }
          break;

        case CodeBlockStyle:
          rval
            << ((value & 0x01) ? "" : "no ") << "sel arthm/"
            << ((value & 0x02) ? "" : "no ") << "cntxt prob reset/" 
            << ((value & 0x04) ? "" : "no ") << "coding pass term/"
            << ((value & 0x08) ? "" : "no ") << "term vert csl/"
            << ((value & 0x10) ? "" : "no ") << "pred term/"
            << ((value & 0x20) ? "" : "no ") << "seg sym";
          break;
      }

      return rval.str();
    }

  private:
    Field _field;
};

#endif // _XFRM_COD_H_

