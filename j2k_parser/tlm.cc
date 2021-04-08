#include "tlm.h"
#include "formatter.h"
#include "xfrm_tlm.h"

void TLM::init(int fd)
{
  MarkerSegment::init(fd, Marker::TLM );

  _Z    = get_field(fd, 1, "Ztlm");
  _S    = get_field(fd, 1, "Stlm");

  _ST = (_S & 0x30) >> 4;
  _SP = 4*(1 + (_S & 0x40) >> 6);

  uint8_t rec_length = _ST + _SP;
  uint16_t nrec = (_size - 4)/rec_length;

  for(int i=0; i<nrec; ++i)
  {
    _T.push_back( get_field(fd, _ST, "Ttlm") );
    _P.push_back( get_field(fd, _SP, "Ptlm") );
  }
}


void TLM::display(std::ostream &s) const
{
  Xfrm_Stlm stlm_xfrm;

  Formatter out(s);
  out.display(_size, 2, "Ltlm");
  out.display(_Z,    1, "Ztlm", "TLM index");
  out.display(_S,    1, "Stlm", stlm_xfrm);

  for(uint16_t i=0; i<_P.size(); ++i)
  {
    out.display(_T.at(i), _ST, "Ttlm", i, "tile index of tile-part");
    out.display(_P.at(i), _SP, "Ptlm", i, "offset from start of SOT to tile-part");
  }
}

