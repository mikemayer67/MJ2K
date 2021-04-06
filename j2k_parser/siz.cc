#include "siz.h"
#include "formatter.h"
#include "xfrm_siz.h"

#include <iostream>

void SIZ::init(int fd)
{
  _size   = get_field(fd, 2, "Lsiz");

  _R   = get_field(fd, 2, "Rsiz");
  _X   = get_field(fd, 4, "Xsiz");
  _Y   = get_field(fd, 4, "Ysiz");
  _XO  = get_field(fd, 4, "XOsiz");
  _YO  = get_field(fd, 4, "YOsiz");
  _XT  = get_field(fd, 4, "XTsiz");
  _YT  = get_field(fd, 4, "YTsiz");
  _XTO = get_field(fd, 4, "XTOsiz");
  _YTO = get_field(fd, 4, "YTOsiz");
  _C   = get_field(fd, 2, "Csiz");

  for(uint16_t i=0; i<_C; ++i)
  {
    CompData comp;
    comp.S  = get_field(fd, 1, "Ssiz");
    comp.XR = get_field(fd, 1, "XRsiz");
    comp.YR = get_field(fd, 1, "YRsiz");

    _compData.push_back(comp);
  }
}

void SIZ::display(std::ostream &s) const
{
  s << "SIZ" << std::endl;

  Formatter out(s);
  out.display(_size,   2, "Lsiz");
  out.display(_R,   2, "Rsiz",   "codestream capabilities");
  out.display(_X,   4, "Xsiz",   "width of reference grid");
  out.display(_Y,   4, "Ysiz",   "height of reference grid");
  out.display(_XO,  4, "XOsiz",  "horizontal offset from ref. grid origin to left side of image");
  out.display(_YO,  4, "YOsiz",  "vertical offset from ref. grid origin to top of image");
  out.display(_XT,  4, "XTsiz",  "width of one tile wrt ref. grid");
  out.display(_YT,  4, "YTsiz",  "height of one tile wrt ref. grid");
  out.display(_XTO, 4, "XTOsiz", "horizontal offset from ref. grid origin to left side of first tile");
  out.display(_YTO, 4, "YTOsiz", "vertical offset from ref. grid origin to top of first tile");
  out.display(_C,   2, "Csiz",   "number of components in the field");

  Xfrm_Ssiz ssiz_xfrm;

  for(int i=0; i<_compData.size(); ++i)
  {
    const CompData &cd = _compData.at(i);
    out.display(cd.S,  1, "Ssiz",  i, ssiz_xfrm);
    out.display(cd.XR, 1, "XRsiz", i, "horiz. sample separation in ith comp wrt the ref.grid");
    out.display(cd.YR, 1, "YRsiz", i, "vert. sample separation in ith comp wrt the ref.grid");
  }
}
