#include "tilepart.h"

#include "marker.h"
#include "cod.h"
#include "coc.h"
#include "qcd.h"
#include "qcc.h"
#include "rgn.h"
#include "poc.h"
#include "ppt.h"
#include "plt.h"
#include "com.h"

#include <iostream>
#include <sstream>
#include <unistd.h>

void J2KTilePart::init(int fd, const SIZ &siz)
{
  Marker marker;

  // first marker in tile part must be SOT
  //   if not, it will throw an exception, so no need to verify that here

  _sot.init(fd);

  bool done = false;
  while( !done )
  {
    marker = Marker(fd);
    switch(marker.code())
    {
      // First tile-part only
      case Marker::COD: _segments.push_back( new COD(fd)     ); break;
      case Marker::COC: _segments.push_back( new COC(fd,siz) ); break;
      case Marker::QCD: _segments.push_back( new QCD(fd)     ); break;
      case Marker::QCC: _segments.push_back( new QCC(fd,siz) ); break;
      case Marker::RGN: _segments.push_back( new RGN(fd,siz) ); break;

      // Any tile-part
      case Marker::POC: _segments.push_back( new POC(fd,siz) ); break;
      case Marker::PPT: _segments.push_back( new PPT(fd)     ); break;
      case Marker::PLT: _segments.push_back( new PLT(fd)     ); break;
      case Marker::COM: _segments.push_back( new COM(fd)     ); break;

      // Final marker in tile-part
      case Marker::SOD:
        done = true;
        break;

      // unallowed Markers in tile part (fallthrough is intentional)
      case Marker::SOC:
      case Marker::SOT:
      case Marker::EOC:
      case Marker::SIZ:
      case Marker::TLM:
      case Marker::PLM:
      case Marker::PPM:
      case Marker::SOP:
      case Marker::EPH:
      case Marker::CRG:
        {
          std::stringstream err;
          err << marker.str() << "not allowed in tile part";
          throw err.str();
        }

      case 0xff30: case 0xff31: case 0xff32: case 0xff33:
      case 0xff34: case 0xff35: case 0xff36: case 0xff37:
      case 0xff38: case 0xff39: case 0xff3a: case 0xff3b:
      case 0xff3c: case 0xff3d: case 0xff3e: case 0xff3f: 
        std::cout << "Skipping marker: " << std::hex << marker.code() << std::endl;
        marker = Marker(fd,true);  // consume the skipped marker
        break;
    }
  }

  // Tile part must end with a SOD marker
  _sod = Marker(fd, Marker::SOD);

  uint64_t eod = _sot.end_of_data();
  uint64_t eof = lseek(fd,0,SEEK_END);

  if( eod > eof - 2 ) // need to account for final EOC marker
  {
    uint32_t nbytes = _sot.Psot();
    uint64_t shortfall = eod + 2 - eof;
    uint32_t available = nbytes - shortfall;

    std::stringstream err;
    err << "Insufficient data in file for tile-part " << uint16_t(_sot.TNsot()) 
      << " of tile " << uint16_t(_sot.TPsot()) << " (" << nbytes << " needed, "
      << available << " available)";
    throw err.str();
  }
  lseek(fd,eod,SEEK_SET);
}

void J2KTilePart::display(std::ostream &s) const
{
  s << _sot;

  for(uint16_t i=0; i<_segments.size(); ++i)
  {
    s << *(_segments.at(i));
  }

  s << _sod
    << "----------- " << std::endl
    << std::dec << _sot.Psot() << " bytes of tile-part data" << std::endl
    << "----------- " << std::endl;
}
