#ifndef _MARKER_H_
#define _MARKER_H_

#include <string>
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

      // Valid but ignored markers
      IGN30 = 0xff30,
      IGN31 = 0xff31,
      IGN32 = 0xff32,
      IGN33 = 0xff33,
      IGN34 = 0xff34,
      IGN35 = 0xff35,
      IGN36 = 0xff36,
      IGN37 = 0xff37,
      IGN38 = 0xff38,
      IGN39 = 0xff39,
      IGN3A = 0xff3a,
      IGN3B = 0xff3b,
      IGN3C = 0xff3c,
      IGN3D = 0xff3d,
      IGN3E = 0xff3e,
      IGN3F = 0xff3f,
      
      // Defaut value indicating uninitialized marker
      EMPTY = 0x0000
    };

    static std::string code_to_str(Code_t);

    std::string str(void) const { return code_to_str(_code); }
    
  public:
    Marker(Code_t code=EMPTY) : _code(code) {}

    // If the expected code is not found, this will throw an exception
    Marker(int fd, Code_t expected);

    // Note that if advance is false, this is a look ahead operation (file pointer is not advanced)
    Marker(int fd, bool advance=false);

    void display(std::ostream &s) const;

    Code_t   code   (void) const { return _code; }
    uint64_t offset (void) const { return _offset; }

    bool is(Code_t code) const { return code == _code; }

  private:

    void _read(int fd);

    Code_t   _code;
    uint64_t _offset;
};

static std::ostream &operator<<(std::ostream &s, const Marker &x) {x.display(s); return s;}

#endif // _MARKER_H_
