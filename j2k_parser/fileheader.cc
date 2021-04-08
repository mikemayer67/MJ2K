#include "fileheader.h"

#include "marker.h"
#include "cod.h"
#include "coc.h"
#include "qcd.h"
#include "qcc.h"
#include "rgn.h"
#include "poc.h"
#include "ppm.h"
#include "tlm.h"
#include "plm.h"
#include "crg.h"
#include "com.h"

#include <sstream>

void J2KFileHeader::init(int fd)
{
  Marker marker;

  // first two markers in file must be SOC and SIZ (in that order)
  _soc = Marker(fd,Marker::SOC);
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

      case Marker::COD: _segments.push_back( new COD(fd)      ); break;
      case Marker::COC: _segments.push_back( new COC(fd,_siz) ); break;
      case Marker::QCD: _segments.push_back( new QCD(fd)      ); break;
      case Marker::QCC: _segments.push_back( new QCC(fd,_siz) ); break;
      case Marker::COM: _segments.push_back( new COM(fd)      ); break;
      case Marker::RGN: _segments.push_back( new RGN(fd,_siz) ); break;
      case Marker::POC: _segments.push_back( new POC(fd,_siz) ); break;
      case Marker::PPM: _segments.push_back( new PPM(fd)      ); break;
      case Marker::TLM: _segments.push_back( new TLM(fd)      ); break;
      case Marker::PLM: _segments.push_back( new PLM(fd)      ); break;
      case Marker::CRG: _segments.push_back( new CRG(fd)      ); break;

      // unallowed Markers in file header (fallthrough is intentional)
      case Marker::SOC:
      case Marker::SOD:
      case Marker::PLT:
      case Marker::PPT:
      case Marker::SOP:
      case Marker::EPH:
        {
          std::stringstream err;
          err << marker.str() << "not allowed in file header";
          throw err.str();
        }


      case 0xff30: case 0xff31: case 0xff32: case 0xff33:
      case 0xff34: case 0xff35: case 0xff36: case 0xff37:
      case 0xff38: case 0xff39: case 0xff3a: case 0xff3b:
      case 0xff3c: case 0xff3d: case 0xff3e: case 0xff3f: 
        std::cout << "Skipping marker: " << std::hex << marker.code() << std::endl;
        marker = Marker(fd,true);  // consume the skipped marker
        break;

      default:
        std::stringstream err;
        err << "Unknown marker: 0x" << std::hex << marker.code();
        throw err.str();
        break;
    }
  }

}

void J2KFileHeader::display(std::ostream &s) const
{
  s << _soc << _siz;

  for(uint16_t i=0; i<_segments.size(); ++i)
  {
    s << *(_segments.at(i));
  }
}


