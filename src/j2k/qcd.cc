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

#include "qcd.h"
#include "formatter.h"
#include "xfrm_qcd.h"

void QCD::init(int fd)
{
  MarkerSegment::init(fd, Marker::QCD );

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

