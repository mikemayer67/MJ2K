#include "fileheader.h"

#include "marker.h"
#include "cod.h"
#include "coc.h"
#include "qcd.h"
#include "qcc.h"
#include "rgn.h"
#include "poc.h"
#include "com.h"

#include <sstream>
#include <vector>

#include <unistd.h>
static uint16_t tmp_size(0);
#define SKIP(k) \
  std::cout << "Skipping " << k << std::endl; \
  read(fd,&tmp_size,2); \
  std::cout << "  " << tmp_size << " bytes" << std::endl; \
  tmp_size = ((tmp_size&0x00ff)<<8)|((tmp_size&0xff00)>>8); \
  lseek(fd, tmp_size-2, SEEK_CUR);

void J2KFileHeader::init(int fd)
{
  Marker marker;

  // verify SOC
  marker = Marker(fd);
  if( !marker.is(Marker::SOC) ) throw "First marker is not SOC";

  // verify/parse SIZ
  marker = Marker(fd);
  if( !marker.is(Marker::SIZ) ) throw "Second marker is not SIZ";
  _siz.init(fd);

  bool done = false;
  while( !done )
  {
    marker = Marker(fd);
    switch(marker.code())
    {
      case Marker::SIZ: throw "only one SIZ allowed in header"; break;

      case Marker::EOC: done = true; break;
      case Marker::SOT: done = true; break;

      case Marker::PPM: SKIP("PPM") break;
      case Marker::TLM: SKIP("TLM") break;
      case Marker::PLM: SKIP("PLM") break;
      case Marker::CRG: SKIP("CRG") break;

      case Marker::COD: _segments.push_back( new COD(fd)      ); break;
      case Marker::COC: _segments.push_back( new COC(fd,_siz) ); break;
      case Marker::QCD: _segments.push_back( new QCD(fd)      ); break;
      case Marker::QCC: _segments.push_back( new QCC(fd,_siz) ); break;
      case Marker::COM: _segments.push_back( new COM(fd)      ); break;
      case Marker::RGN: _segments.push_back( new RGN(fd,_siz) ); break;
      case Marker::POC: _segments.push_back( new POC(fd,_siz) ); break;

      case Marker::SOC: throw "SOC not allowed in header"; break;
      case Marker::SOD: throw "SOD not allowed in header"; break;
      case Marker::PLT: throw "PLT not allowed in header"; break;
      case Marker::PPT: throw "PPT not allowed in header"; break;
      case Marker::SOP: throw "SOP not allowed in header"; break;
      case Marker::EPH: throw "EPH not allowed in header"; break;

      case 0xff30: case 0xff31: case 0xff32: case 0xff33:
      case 0xff34: case 0xff35: case 0xff36: case 0xff37:
      case 0xff38: case 0xff39: case 0xff3a: case 0xff3b:
      case 0xff3c: case 0xff3d: case 0xff3e: case 0xff3f: 
        std::cout << "Skipping marker: " << std::hex << marker.code() << std::endl;
        break;

      default:
        std::stringstream err;
        err << "Unknown marker: " << std::hex << marker.code();
        throw err.str();
        break;
    }
  }

}

void J2KFileHeader::display(std::ostream &s) const
{
  s << "SOC" << std::endl
    << _siz;

  std::vector<MarkerSegment*>::const_iterator seg;
  for(seg=_segments.begin(); seg!=_segments.end(); ++seg)
  {
    s << **seg;
  }
  s << std::endl;
}


