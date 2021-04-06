#ifndef _COC_H_
#define _COC_H_

#include "segment.h"
#include "siz.h"
#include "cod.h"

#include <vector>

//------------------------------------------------------------------------------
// COC - Coding Style Component
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

//----------------------------------------
// Table A-15 SPcoc
//----------------------------------------
//  8 bits  : number of decomposition levels
//  8 bits  : code-block width exponent offset (xcb)
//  8 bits  : code-block height exponent offset (ycb)
//  8 bits  : style of the code-block coding passes
//  8 bits  : wavelet transform used
// variable : precint width/heigh (if Scod/Scoc != xxxxxxx0)
//----------------------------------------

class COC : public MarkerSegment
{
  public:
    COC(void)                   : MarkerSegment(Marker::COC) {}
    COC(int fd, const SIZ &siz) : MarkerSegment(Marker::COC) { init(fd,siz); }

    void init(int fd, const SIZ &siz);
    void display(std::ostream &s) const;

  public:

    typedef COD::SPcod_t SPcoc_t;

    uint16_t Lcoc   (void) const { return _size; }
    uint16_t Ccoc   (void) const { return _C;   }
    uint16_t Scoc   (void) const { return _S;   }

    const SPcoc_t &SPcoc(void) const { return _SP; }

    uint8_t  num_decomp_levels (void) const { return _SP.num_decomp_levels; } 
    uint8_t  code_block_width  (void) const { return _SP.code_block_width; } 
    uint8_t  code_block_height (void) const { return _SP.code_block_height; } 
    uint8_t  code_block_style  (void) const { return _SP.code_block_style; } 
    uint8_t  wavelet           (void) const { return _SP.wavelet; } 

    uint8_t precint_width(uint16_t i) const {
      try { return _SP.precinct_width.at(i); }
      catch(std::out_of_range e) { throw_range_error("SPcoc.precinct_width",i,_SP.precinct_width.size()); }
    }

  private:
    uint8_t  _lenC;

    uint16_t _C;
    uint8_t  _S;
    SPcoc_t  _SP;
};

#endif // _COC_H_ 
