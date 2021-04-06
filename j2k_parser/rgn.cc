#include "rgn.h"

#include "formatter.h"

#include <iostream>

void RGN::init(int fd, const SIZ &siz)
{
  _lenC = (siz.Csiz() < 257 ? 1 : 2);

  _size = get_field(fd, 2,     "Lrgn");
  _C    = get_field(fd, _lenC, "Crgn");
  _S    = get_field(fd, 1,     "Srgn");
  _SP   = get_field(fd, 1,     "SPrgn");
}

void RGN::display(std::ostream &s) const
{
  s << "RGN" << std::endl;

  Formatter out(s);
  out.display(_size,                 2, "Lrgn");
  out.display(_C,                _lenC, "Crgn",  "component index");
  out.display(_S,                    1, "Srgn",  "0=implicit ROI");
  out.display(_SP,                   1, "SPrgn", "binary shift of ROI coefficients");
}
