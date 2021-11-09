#include "poc.h"

#include "formatter.h"
#include "xfrm_cod.h"

void POC::init(int fd, const SIZ &siz)
{
  MarkerSegment::init(fd, Marker::POC );

  _lenC = (siz.Csiz() < 257 ? 1 : 2);

  int n = (_size-2)/(5+2*_lenC);

  for(uint16_t i=0; i<n; ++i)
  {
    ProgOrderChange_t poc;

    poc.RS  = get_field(fd, 1,     "RSpoc");
    poc.CS  = get_field(fd, _lenC, "CSpoc");
    poc.LYE = get_field(fd, 2,     "LYESpoc");
    poc.RE  = get_field(fd, 1,     "REpoc");
    poc.CE  = get_field(fd, _lenC, "CEpoc");
    poc.P   = get_field(fd, 1,     "Ppoc");

    _progOrderChange.push_back(poc);
  }
}

void POC::display(std::ostream &s) const
{
  // yes, this is hackish, but the progression order format is same as for SGcod
  Xfrm_SGcod ppoc_xfrm(Xfrm_SGcod::ProgOrder);

  Formatter out(s);
  out.display(_size,                 2, "Lpoc");

  for(uint16_t i=0; i<_progOrderChange.size(); ++i)
  {
    const ProgOrderChange_t &poc = _progOrderChange.at(i);
    out.display( poc.RS,      1, "RSpoc",  i, "resolution level index at start of progression" );
    out.display( poc.CS,  _lenC, "CSpoc",  i, "component index at start of progression" );
    out.display( poc.LYE,     2, "LYEpoc", i, "layer index after end of progression" );
    out.display( poc.RE,      1, "REpoc",  i, "resolution level index after end of progression" );
    out.display( poc.CE,  _lenC, "CEpoc",  i, "component index after end of progression" );
    out.display( poc.P,       1, "Ppoc",   i, ppoc_xfrm, "progression order" );
  }
}
