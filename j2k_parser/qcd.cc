#include "qcd.h"
#include "formatter.h"
#include "xfrm_qcd.h"

#include <iostream>

void QCD::init(int fd)
{
  _size = get_field(fd, 2, "Lqcd");
  _S    = get_field(fd, 1, "Sqcd");

  int nbytes(0);
  if ( (_S & 0x1f) == 0x00 ) { nbytes = 1; } // no quantization
  else                       { nbytes = 2; }

  uint16_t n = (_size-3)/nbytes;
  for(uint16_t i=0; i<n; ++i)
  {
    _SP.push_back( get_field(fd,nbytes,"SPqcd") );
  }
}

void QCD::display(std::ostream &s) const
{
  Xfrm_Sqcd sqcd_xfrm;

  s << "QCD" << std::endl;

  Formatter out(s);
  out.display(_size,   2, "Lqcd");
  out.display(_S, 1, "Sqcd", sqcd_xfrm);

  Xfrm_SPqcd spqcd_xfrm(_S);

  int nbytes(0);
  std::string info;

  if( (_S & 0x1f) == 0 ) // no quantization
  {
    nbytes = 1;
    info = "Exponent (epsilon_b)";
  }
  else
  {
    nbytes = 2;
    info = "Exponent (epsilon_b), Mantissa (mu_b)";
  }

  for(int i=0; i<_SP.size(); ++i)
  {
    out.display(_SP.at(i), nbytes, "SPqcd", i, spqcd_xfrm, info); 
  }
}

