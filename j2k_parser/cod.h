#ifndef _COD_H_
#define _COD_H_

#include "segment.h"

#include <vector>

//------------------------------------------------------------------------------
// COD - Coding Style Default
//   Functional marker
//   File Header: requried
//   Tile Header: optional
//------------------------------------------------------------------------------

//----------------------------------------
// Table A-14 SGcod
//----------------------------------------
// bits  0- 7 : progression order (Table A-16)
// bits  8-23 : number of layers
// bits 24-31 : multi-component transform (Table A-17)
//----------------------------------------

//----------------------------------------
// Table A-15 SPcod
//----------------------------------------
//  8 bits  : number of decomposition levels
//  8 bits  : code-block width exponent offset (xcb)
//  8 bits  : code-block height exponent offset (ycb)
//  8 bits  : style of the code-block coding passes
//  8 bits  : wavelet transform used
// variable : precint width/heigh (if Scod/Scoc != xxxxxxx0)
//----------------------------------------

class COD : public MarkerSegment
{
  public:
    COD(void)   {}
    COD(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

  public:
    struct SGcod_t
    {
      uint8_t  prog_order;
      uint16_t num_layers;
      uint8_t  mcomp_xform;
    };

    struct SPcod_t
    {
      uint8_t num_decomp_levels;
      uint8_t code_block_width;
      uint8_t code_block_height;
      uint8_t code_block_style;
      uint8_t wavelet;

      std::vector<uint8_t> precinct_width;
    };

    uint16_t Lcod   (void) const { return _size; }
    uint16_t Scod   (void) const { return _S;   }

    const SGcod_t &SGcod(void) const { return _SG; }
    const SPcod_t &SPcod(void) const { return _SP; }

    uint8_t  prog_order        (void) const { return _SG.prog_order; }
    uint16_t num_layers        (void) const { return _SG.num_layers; }
    uint8_t  mcomp_xform       (void) const { return _SG.mcomp_xform; }
    uint8_t  num_decomp_levels (void) const { return _SP.num_decomp_levels; } 
    uint8_t  code_block_width  (void) const { return _SP.code_block_width; } 
    uint8_t  code_block_height (void) const { return _SP.code_block_height; } 
    uint8_t  code_block_style  (void) const { return _SP.code_block_style; } 
    uint8_t  wavelet           (void) const { return _SP.wavelet; } 

    uint8_t precint_width(uint16_t i) const { return _SP.precinct_width.at(i); }

  private:
    uint8_t _S;
    SGcod_t _SG;
    SPcod_t _SP;
};

#endif // _COD_H_ 
