#include <iostream>
#include <sstream>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>

#include <vector>
#include <string>

using std::string;
using std::stringstream;
using std::cout;
using std::endl;
using std::setw;
using std::setfill;
using std::vector;

// J2K codestreams are encoded big-endian.  In order to support both big-endian and little-endian native formats,
//   either the __BIG_ENDIAN__ or __LITTLE_ENDIAN__ macro must be defined.  This is true for most compilers 
//   automatically.  But if not, we want to catch that here...

#ifdef __BIG_ENDIAN__
#ifdef __LITTLE_ENDIAN__
#error Should not set both __BIG_ENDIAN__ and __LITTLE_ENDIAN__
#endif
#else
#ifndef __LITTLE_ENDIAN__
#error Need to compile with either -D__LITTLE_ENDIAN__ or -D__BIG_ENDIAN__
#endif
#endif

#define VALUE_WIDTH 12

//------------------------------------------------------------------------------
// Table A-2 -- List of marker segments
//------------------------------------------------------------------------------

typedef uint16_t MarkerCode_t;

// Main header / tile-part header marker usage
// R = required, O = optional, N = not allowed, L = last marker
// Deliminting markers
//const MarkerCode_t SOC = 0xff4f ; //  R N  Start of codestream
const MarkerCode_t SOT = 0xff90 ; //  N R  Start of tile part
const MarkerCode_t SOD = 0xff93 ; //  N L  Start of data
const MarkerCode_t EOC = 0xffd9 ; //  N N  End of codestream
// Fixed info markers
//const MarkerCode_t SIZ = 0xff51 ; //  R N  Image and tile size
// Functional markers
//const MarkerCode_t COD = 0xff52 ; //  R O  coding style default
const MarkerCode_t COC = 0xff53 ; //  O O  coding style component
const MarkerCode_t RGN = 0xff5e ; //  O O  region of interest
//const MarkerCode_t QCD = 0xff5c ; //  O O  quantization default
//const MarkerCode_t QCC = 0xff5d ; //  O O  quantization component
const MarkerCode_t POD = 0xff5f ; //  O O  prgression order default
// Pointer markers
const MarkerCode_t TLM = 0xff55 ; //  O N  tile part lengths, main
const MarkerCode_t PLM = 0xff57 ; //  O N  packet length, main
const MarkerCode_t PLT = 0xff58 ; //  N O  packet length, tile
const MarkerCode_t PPM = 0xff60 ; //  O N  packed packet, main
const MarkerCode_t PPT = 0xff61 ; //  N O  packed packet, tile
// In bit stream markers
const MarkerCode_t SOP = 0xff91 ; //  N O  start of packet (bit stream)
const MarkerCode_t EPH = 0xff92 ; //  N O  end of packet (bit stream)
// Information markers
const MarkerCode_t CME = 0xff64 ; //  O O  comment and extension

//------------------------------------------------------------------------------
// Marker Field parsers
//------------------------------------------------------------------------------

class StringValue
{
  public:
    virtual string operator()(uint32_t value) const { return std::to_string(value); }

  protected:
    string indent() const { 
      stringstream rval;
      rval << setw(4+VALUE_WIDTH) << "---->    ";
      return rval.str();
    }
};


MarkerCode_t read_marker(int fd)
{
  MarkerCode_t marker;
  read(fd, &marker, sizeof(MarkerCode_t));

#ifdef __LITTLE_ENDIAN__
  marker = ((marker & 0x00ff)<<8) | ((marker & 0xff00)>>8);
#endif

  return marker;
}

uint32_t parse_marker_field(int fd, int N, string parameter, int index, string description, StringValue &cvt)
{
  uint8_t byte;
  uint32_t value(0);

  for(uint16_t i=0; i<N; ++i)
  {
    read(fd, &byte, 1);

#ifdef __BIG_ENDIAN__
    value <<= 8;
    value |=  byte;
#else
    uint32_t tmp = byte;
    value |= ( tmp << 8*(N-i-1) );
#endif
  }

  if(index>=0) {
    stringstream p;
    p << parameter << "." << 1+index;
    parameter = p.str();

    if(index>0) description="";
  }

  cout 
    << "  " << setw(10) << parameter << ": "
    << string(8-2*N,'.')
    << setw(2*N) << setfill('0') << std::hex << uint32_t(value) << setfill(' ')
    << setw(VALUE_WIDTH) << cvt(value) << "    "
    << description
    <<endl;

  return value;
}

uint32_t parse_marker_field(int fd, int N, string parameter, string description="")
{ 
  StringValue cvt;
  return parse_marker_field(fd, N, parameter, -1, description, cvt); 
}

uint32_t parse_marker_field(int fd, int N, string parameter, StringValue &cvt, string description="")
{ 
  return parse_marker_field(fd, N, parameter, -1, description, cvt); 
}

uint32_t parse_marker_field(int fd, int N, string parameter, int index, string description="")
{ 
  StringValue cvt;
  return parse_marker_field(fd, N, parameter, index, description, cvt); 
}

uint32_t parse_marker_field(int fd, int N, string parameter, int index, StringValue &cvt, string description="")
{ 
  return parse_marker_field(fd, N, parameter, index, description, cvt); 
}


//------------------------------------------------------------------------------
// Marker parsers
//------------------------------------------------------------------------------

class Marker
{
  public:
    Marker(void) : size(0) {}

  protected:
    uint16_t size;
};

typedef Marker               *MarkerPtr_t;
typedef vector<MarkerPtr_t>   MarkerList_t;

//------------------------------------------------------------------------------
// SOC - Start of codestream
//   Delimiting marker
//   File Header: required (very first marker)
//   Tile Header: not allowed
//------------------------------------------------------------------------------
class SOC : public Marker
{
  public:
    static const MarkerCode_t Code = 0xff4f;

  public:
    SOC(int fd) : Marker()
  {
    cout << "SOC" << endl;
  }
};

//------------------------------------------------------------------------------
// SIZ - Image and tile size
//   Fixed info marker
//   File Header: required (must immediately follow SOC)
//   Tile Header: not allowed
//------------------------------------------------------------------------------

//----------------------------------------
// Table A-11 Ssiz
//----------------------------------------
// x0000000 - x0100101: componenents are value+1: from 1 bith deep through 38 bits deep
// 0xxxxxxx           : components are unsigned values
// 1xxxxxxx           : components are   signed values
//----------------------------------------

class SsizConverter : public StringValue {
  public:
    string operator()(uint32_t Ssiz) const
    {
      uint8_t bits = Ssiz & 0x7f; 
      uint8_t s    = Ssiz & 0x80;

      stringstream rval;
      rval << indent();
      rval << "precision (";
      if( s == 0 )
      {
        uint32_t lim = (1L << bits) - 1;
        rval << "0 - " << lim;
      }
      else
      {
        uint32_t lim = (1L << (bits-1));
        rval << "-" << lim << " - " << lim-1;
      }
      rval << ")";

      return rval.str();
    }
};

class SIZ : public Marker
{
  public:
    static const MarkerCode_t Code = 0xff51;

  public:
    SIZ(int fd) : Marker(), comp(NULL)
    {
      SsizConverter ssiz_cvt;

      cout << "SIZ" << endl;

      size   = parse_marker_field(fd, 2, "Lsiz");

      Rsiz   = parse_marker_field(fd, 2, "Rsiz",   "codestream capabilities");
      Xsiz   = parse_marker_field(fd, 4, "Xsiz",   "width of reference grid");
      Ysiz   = parse_marker_field(fd, 4, "Ysiz",   "height of reference grid");
      XOsiz  = parse_marker_field(fd, 4, "XOsiz",  "horizontal offset from ref. grid origin to left side of image");
      YOsiz  = parse_marker_field(fd, 4, "YOsiz",  "vertical offset from ref. grid origin to top of image");
      XTsiz  = parse_marker_field(fd, 4, "XTsiz",  "width of one tile wrt ref. grid");
      YTsiz  = parse_marker_field(fd, 4, "YTsiz",  "height of one tile wrt ref. grid");
      XTOsiz = parse_marker_field(fd, 4, "XTOsiz", "horizontal offset from ref. grid origin to left side of first tile");
      YTOsiz = parse_marker_field(fd, 4, "YTOsiz", "vertical offset from ref. grid origin to top of first tile");
      Csiz   = parse_marker_field(fd, 2, "Csiz",   "number of components in the field");

      comp = new CompData[Csiz];
      for(uint16_t i=0; i<Csiz; ++i)
      {
        comp[i].Ssiz  = parse_marker_field(fd, 1, "Ssiz",  i, ssiz_cvt);
        comp[i].XRsiz = parse_marker_field(fd, 1, "XRsiz", i, "horizontal separation of samples in ith component wrt the ref. grid");
        comp[i].YRsiz = parse_marker_field(fd, 1, "YRsiz", i, "vertical separation of samples in ith component wrt the ref. grid");
      }
    }

    ~SIZ() { delete[] comp; }

  public:
    uint16_t Rsiz;
    uint32_t Xsiz;
    uint32_t Ysiz;
    uint32_t XOsiz;
    uint32_t YOsiz;
    uint32_t XTsiz;
    uint32_t YTsiz;
    uint32_t XTOsiz;
    uint32_t YTOsiz;
    uint16_t Csiz;

    struct CompData
    {
      uint8_t Ssiz;
      uint8_t XRsiz;
      uint8_t YRsiz;
    };

    CompData *comp;
};


//------------------------------------------------------------------------------
// QCD - Quanitization Defaut
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

//----------------------------------------
// Table A-28 Sqcd (and Sqcc)
//----------------------------------------
// xxx00000            : no quantication                spqcd = 8bits/Table A-29
// xxx00001            : scalar derived (Equation E.5)  spqcd = 16bits/Table A-30
// xxx00010            : scalar expounded               spqcd = 16bits/Table A-30
// 000xxxxx - 111xxxxx : number of guard bits
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

class SqcdConverter : public StringValue {
  public:
    string operator()(uint32_t Sqcd) const
    {
      uint8_t type = Sqcd & 0x1f;
      uint8_t bits = (Sqcd & 0xe0) >> 5;

      stringstream rval;
      rval << indent();
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
          std::cerr << "Unknown Sqcd value (" << std::hex << uint16_t(type) << ")";
          exit(1);
          break;
      }

      rval << "  (" << uint16_t(bits) << " guard bits)";
      return rval.str();
    }
};

class SPqcdConverter_noq : public StringValue {
  public:
    string operator()(uint32_t SPqcd) const
    {
      uint8_t exponent = (SPqcd & 0xf8) >> 3;
      stringstream rval;
      rval << uint16_t(exponent);
      return rval.str();
    }
};

class SPqcdConverter_q : public StringValue {
  public:
    string operator()(uint32_t SPqcd) const
    {
      uint16_t mantissa = SPqcd & 0x07ff;
      uint16_t exponent = (SPqcd & 0xf800) >> 11;
      stringstream rval;
      rval << "(" << uint16_t(exponent) << "," << uint16_t(mantissa) << ")";
      return rval.str();
    }
};

class QCD : public Marker
{
  public:
    static const MarkerCode_t Code = 0xff5c;

  public:
    QCD(int fd) : Marker(), SPqcd(NULL) {
      SqcdConverter sqcd_cvt;

      cout << "QCD" << endl;

      size   = parse_marker_field(fd, 2, "Lqcd");
      Sqcd   = parse_marker_field(fd, 1, "Sqcd", sqcd_cvt); 

      if( (Sqcd & 0x1f) == 0x00 ) // no_quantization
      {
        SPqcdConverter_noq spqcd_cvt;

        int n = size-3;
        SPqcd = new uint16_t[n];
        for (int i=0; i<n; ++i) {
          SPqcd[i] = parse_marker_field(fd, 1, "SPqcd", i, "Exponent (epsilon_b)", spqcd_cvt); 
        }
      }
      else
      {
        SPqcdConverter_q spqcd_cvt;
        int n = (size-3)/2;
        SPqcd = new uint16_t[n];
        for (int i=0; i<n; ++i) {
          SPqcd[i] = parse_marker_field(fd, 2, "SPqcd", i, "Exponent (epsilon_b), Mantissa (mu_b)", spqcd_cvt); 
        }
      }
    }

    ~QCD() { delete[] SPqcd; }

  private:

    uint8_t   Sqcd;
    uint16_t *SPqcd;
};

//------------------------------------------------------------------------------
// QCC - Quanitization Component
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------


class QCC : public Marker
{
  public:
    static const MarkerCode_t Code = 0xff5d;

  public:
    QCC(int fd, const SIZ *siz) : Marker(), SPqcc(NULL) {
      SqcdConverter sqcc_cvt;

      cout << "QCC" << endl;

      size   = parse_marker_field(fd, 2, "Lqcc");

      if(siz->Csiz < 257) {
        Cqcc = parse_marker_field(fd, 1, "Cqcc", "component index");
      } else {
        Cqcc = parse_marker_field(fd, 2, "Cqcc", "component index");
      }

      Sqcc   = parse_marker_field(fd, 1, "Sqcc", sqcc_cvt); 

      if( (Sqcc & 0x1f) == 0x00 ) // no_quantization
      {
        SPqcdConverter_noq spqcc_cvt;

        int n = size-3;
        SPqcc = new uint16_t[n];
        for (int i=0; i<n; ++i) {
          SPqcc[i] = parse_marker_field(fd, 1, "SPqcc", i, "Exponent (epsilon_b)", spqcc_cvt); 
        }
      }
      else
      {
        SPqcdConverter_q spqcc_cvt;
        int n = (size-3)/2;
        SPqcc = new uint16_t[n];
        for (int i=0; i<n; ++i) {
          SPqcc[i] = parse_marker_field(fd, 2, "SPqcc", i, "Exponent (epsilon_b), Mantissa (mu_b)", spqcc_cvt); 
        }
      }
    }

    ~QCC() { delete[] SPqcc; }

  private:

    uint16_t  Cqcc;
    uint8_t   Sqcc;
    uint16_t *SPqcc;
};

//------------------------------------------------------------------------------
// COD - Coding Style Default
//   Functional marker
//   File Header: requried
//   Tile Header: optional
//------------------------------------------------------------------------------

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
// Table A-14 SGcod
//----------------------------------------
// bits  0- 7 : progression order (Table A-16)
// bits  8-23 : number of layers
// bits 24-31 : multi-component transform (Table A-17)
//----------------------------------------
// Table A-16 Progression Order
//----------------------------------------
// 00000000 : layer-resolution level-component-position
// 00000001 : resolution level-layer-component-position
// 00000010 : resolution level-position-component-layer
// 00000011 : position-component-resolution level-layer
// 00000100 : component-position-resolution level-layer
//----------------------------------------
// Table A-17 Multi-component information
//----------------------------------------
// 00000000 : No transform specified
// 00000001 : transform used on components 0, 1, 2 for coding efficiency
//----------------------------------------
// Table A-18 Codeblock width/height
//----------------------------------------
// xxxx0000 - xxxx1000 : code block width/height exponent - 2
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
// Table A-20 Transformation
//----------------------------------------
// 00000000 : 9-7 irreversible filter
// 00000001 : 5-3 reversible filter
//----------------------------------------
// Table A-21 Transformation
//----------------------------------------
// xxxx0000 - xxxx1111 PPx
// 0000xxxx - 1111xxxx PPy
//----------------------------------------

class ScodConverter : public StringValue {
  public:
    string operator()(uint32_t Scod) const
    {
      stringstream rval;
      rval << indent() << "Entry coder, precints " << ((Scod & 0x01) ? "defined below" : "with PPx=PPy=15")
        << ", SOP " << ((Scod & 0x02) ? "may be used" : "not used")
        << ", EPH " << ((Scod & 0x04) ? "may be used" : "not used");
      return rval.str();
    }
};

class SGcodPOConverter : public StringValue {
  public:
    string operator()(uint32_t value) const
    {
      stringstream rval;
      switch(value) {
        case 0: rval << "LRCP"; break;
        case 1: rval << "RLCP"; break;
        case 2: rval << "RPCL"; break;
        case 3: rval << "PCRL"; break;
        case 4: rval << "CPRL"; break;
        default:
          std::cerr << "Unknown progression order: " << value << endl;
          exit(1);
          break;
      }
      return rval.str();
    }
};
      
class SGcodMCXConverter : public StringValue {
  public:
    string operator()(uint32_t value) const
    {
      stringstream rval;
      rval << indent();
      switch(value) {
        case 0: rval << "no multi component transform"; break;
        case 1: rval << "xform used on components 0-2"; break;
        default:
          std::cerr << endl << "Unknown multi component transform: " << value << endl;
          exit(1);
          break;
      }
      return rval.str();
    }
};

class SPcodCBXConverter : public StringValue {
  public:
    string operator()(uint32_t value) const
    {
      uint32_t exp = 2 + (value & 0x0f);
      if( exp > 10 ) {
        std::cerr << endl << "Max codeblock width/height exponent is 10" << endl;
        exit(1);
      }
      return std::to_string(1 << exp);
    }
};

class SPcodCBSConverter : public StringValue {
  public:
    string operator()(uint32_t value) const
    {
      stringstream rval;
      rval << indent()
        << ((value & 0x01) ? "" : "no ") << "sel arthm/"
        << ((value & 0x02) ? "" : "no ") << "cntxt prob reset/" 
        << ((value & 0x04) ? "" : "no ") << "coding pass term/"
        << ((value & 0x08) ? "" : "no ") << "term vert csl/"
        << ((value & 0x10) ? "" : "no ") << "pred term/"
        << ((value & 0x20) ? "" : "no ") << "seg sym";
      return rval.str();
    }
};

class SPcodXFMConverter : public StringValue {
  public:
    string operator()(uint32_t value) const
    {
      return ((value & 0x01) == 0 ? "9-7 VL" : "5-3 NL");
    }
};

class SPcodPSzConverter : public StringValue {
  public:
    string operator()(uint32_t value) const
    {
      uint16_t ppx = (value & 0x0f);
      uint16_t ppy = (value & 0xf0) >> 4;
      stringstream rval;
      rval << "(" << ppx << "," << ppy << ")";
      return rval.str();
    }
};

class COD : public Marker
{
  public:
    static const MarkerCode_t Code = 0xff52;

  public:
    COD(int fd) : Marker(), SPcod_psz(NULL)
    {
      ScodConverter scod_cvt;
      SGcodPOConverter  sgcod_po_cvt;
      SGcodMCXConverter sgcod_mcx_cvt;
      SPcodCBXConverter spcod_cbx_cvt;
      SPcodCBSConverter spcod_cbs_cvt;
      SPcodXFMConverter spcod_xfm_cvt;
      SPcodPSzConverter spcod_psz_cvt;

      cout << "COD" << endl;

      size      = parse_marker_field(fd, 2, "Lcod");

      Scod      = parse_marker_field(fd, 1, "Scod", scod_cvt);
      SGcod_po  = parse_marker_field(fd, 1, "SGcod-A", sgcod_po_cvt, "progression order");
      SGcod_nl  = parse_marker_field(fd, 2, "SCcod-B", "number of layers");
      SGcod_mcx = parse_marker_field(fd, 1, "SCcod-C", sgcod_mcx_cvt);
      SPcod_ndl = parse_marker_field(fd, 1, "SPcod-D", "number of decomposition levels");
      SPcod_cbw = parse_marker_field(fd, 1, "SPcod-E", spcod_cbx_cvt, "code block width");
      SPcod_cbh = parse_marker_field(fd, 1, "SPcod-F", spcod_cbx_cvt, "code block height");
      SPcod_cbs = parse_marker_field(fd, 1, "SPcod-G", spcod_cbs_cvt);
      SPcod_xfm = parse_marker_field(fd, 1, "SPcod-H", spcod_xfm_cvt, "transformation");

      int n = size - 12;
      SPcod_psz = new uint16_t[n];
      for(int i=0; i<n; ++i)
      {
        SPcod_psz[i] = parse_marker_field(fd, 1, "SPcod-I", spcod_psz_cvt, "precinct size (PPx,PPy)");
      }
    }

    ~COD() { delete[] SPcod_psz; }

  private:
    uint8_t  Scod;
    uint32_t SGcod_po;
    uint32_t SGcod_nl;
    uint32_t SGcod_mcx;
    uint32_t SPcod_ndl;
    uint32_t SPcod_cbw;
    uint32_t SPcod_cbh;
    uint32_t SPcod_cbs;
    uint32_t SPcod_xfm;

    uint16_t *SPcod_psz;
};

//------------------------------------------------------------------------------
// MAIN
//------------------------------------------------------------------------------

int main(int argc,char **argv)
{
  if( argc != 2 )
  {
    cout << "Usage:: " << argv[0] << " j2k_filename" << endl;
    exit(1);
  }

  string filename = argv[1];

  int src = open(argv[1],O_RDONLY);
  if( src < 0 )
  {
    cout << "Sorry:: Faild to open " << filename << ": " << strerror(errno) << endl;
  }

  // Markers

  MarkerList_t markers;

  // verify SOC

  MarkerCode_t code = read_marker(src);
  if( code != SOC::Code )
  {
    cout << "Sorry:: " << filename << " does not start with SOC marker" << endl;
    exit(1);
  }
  markers.push_back( new SOC(src) );

  // verify/parse SIZ

  code = read_marker(src);
  if( code != SIZ::Code )
  {
    cout << "Sorry:: " << filename << " does not start with SIZ marker" << endl;
    exit(1);
  }
  SIZ *siz = new SIZ(src);
  markers.push_back(siz);

  // Loop over marker codes until EOC
  bool done = false;
  while(!done)
  {
    MarkerCode_t marker = read_marker(src);

    switch(marker)
    {
      case SIZ::Code: 
        cout << "Errant SIZ marker. There should only be one at the start of the codestream." << endl;
        exit(1);

      case EOC: 
        done = true; 
        break;

      case QCD::Code: markers.push_back( new QCD(src)     ); break;
      case QCC::Code: markers.push_back( new QCC(src,siz) ); break;
      case COD::Code: markers.push_back( new COD(src)     ); break;

      default:
        cout << "Unknown code: " << std::hex << marker << endl;
        exit(1);
        break;
    }
  }

  cout << endl << "DONE" << endl << endl;
}
