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


      case Marker::IGN30: case Marker::IGN31: case Marker::IGN32: case Marker::IGN33:
      case Marker::IGN34: case Marker::IGN35: case Marker::IGN36: case Marker::IGN37:
      case Marker::IGN38: case Marker::IGN39: case Marker::IGN3A: case Marker::IGN3B:
      case Marker::IGN3C: case Marker::IGN3D: case Marker::IGN3E: case Marker::IGN3F: 
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


