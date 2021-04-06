#ifndef _MARKER_H_
#define _MARKER_H_

#include <stdint.h>

//------------------------------------------------------------------------------
// Table A-2 -- List of marker segments
//------------------------------------------------------------------------------

class Marker
{
  public:
    enum Code_t
    {
      // Main header / tile-part header marker usage
      // R = required, O = optional, N = not allowed, L = last marker
      // Deliminting markers
      SOC = 0xff4f, //  R N  Start of codestream
      SOT = 0xff90, //  N R  Start of tile part
      SOD = 0xff93, //  N L  Start of data
      EOC = 0xffd9, //  N N  End of codestream

      // Fixed info markers
      SIZ = 0xff51, //  R N  Image and tile size

      // Functional markers
      COD = 0xff52, //  R O  coding style default
      COC = 0xff53, //  O O  coding style component
      QCD = 0xff5c, //  O O  quantization default
      QCC = 0xff5d, //  O O  quantization component
      RGN = 0xff5e, //  O O  region of interest
      POC = 0xff5f, //  O O  prgression order change

      // Pointer markers
      TLM = 0xff55, //  O N  tile part lengths, main
      PLM = 0xff57, //  O N  packet length, main
      PLT = 0xff58, //  N O  packet length, tile
      PPM = 0xff60, //  O N  packed packet, main
      PPT = 0xff61, //  N O  packed packet, tile

      // In bit stream markers
      SOP = 0xff91, //  N O  start of packet (bit stream)
      EPH = 0xff92, //  N O  end of packet (bit stream)

      // Information markers
      CRG = 0xff63, //  O N  component registration
      COM = 0xff64, //  O O  comment and extension
      
      EMPTY = 0x0000
    };

  public:
    Marker(void) : _code(EMPTY) {}

    Marker(int fd);

    Code_t code(void) const { return _code; }

    bool is(Code_t code) const { 
      return code == _code; 
    }

  protected:
    Code_t _code;
};

#endif // _MARKER_H_
