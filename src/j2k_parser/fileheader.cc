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

extern bool gVerbose;

void J2KFileHeader::init(int fd)
{
  Marker marker;

  // first two markers in file must be SOC and SIZ (in that order)
  _soc = Marker(fd,Marker::SOC);
  _siz.init(fd);

  if(gVerbose) { std::cout << _soc << std::endl << _siz << std::endl; }

  bool done = false;
  while( !done )
  {
    marker = Marker(fd);
    if(gVerbose) { std::cout << marker << std::endl; }

    bool show = false;
    switch(marker.code())
    {
      case Marker::SIZ: throw "only one SIZ allowed in header"; break;

      case Marker::EOC: done = true; break;
      case Marker::SOT: done = true; break;

      case Marker::COD: _segments.push_back( new COD(fd)      ); show=true; break;
      case Marker::COC: _segments.push_back( new COC(fd,_siz) ); show=true; break;
      case Marker::QCD: _segments.push_back( new QCD(fd)      ); show=true; break;
      case Marker::QCC: _segments.push_back( new QCC(fd,_siz) ); show=true; break;
      case Marker::COM: _segments.push_back( new COM(fd)      ); show=true; break;
      case Marker::RGN: _segments.push_back( new RGN(fd,_siz) ); show=true; break;
      case Marker::POC: _segments.push_back( new POC(fd,_siz) ); show=true; break;
      case Marker::PPM: _segments.push_back( new PPM(fd)      ); show=true; break;
      case Marker::TLM: _segments.push_back( new TLM(fd)      ); show=true; break;
      case Marker::PLM: _segments.push_back( new PLM(fd)      ); show=true; break;
      case Marker::CRG: _segments.push_back( new CRG(fd)      ); show=true; break;

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
    if (gVerbose && show) { std::cout << *(_segments.back()) << std::endl; }
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


