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

#include "qcc.h"
#include "formatter.h"
#include "xfrm_qcd.h"  // not a mistake, QCC uses same transformers as QCD

//----------------------------------------
// Table A-31  Quantization component parameters
//----------------------------------------
// QCC      : 16 bits   : 0xFF5D
// Lqcc     : 16 bits   : 5-199
// Cqcc     : 8|16 bits : 0-255 if Csiz<257, 0-16383 if Csiz >= 257
// Sqcc     : 8 bits    : Table A-28
// SPqcc[i] : variable  : Table A-28
// xxx00001            : scalar derived (Equation E.5)  spqcd = 16bits/Table A-30
// xxx00010            : scalar expounded               spqcd = 16bits/Table A-30
// 000xxxxx - 111xxxxx : number of guard bits
//----------------------------------------

void QCC::init(int fd,const SIZ &siz)
{
  MarkerSegment::init(fd, Marker::QCC );

  _lenC = (siz.Csiz() < 257 ? 1 : 2);

  _C    = get_field(fd, _lenC, "Cqcc");
  _S    = get_field(fd, 1,     "Sqcc");

  if ( (_S & 0x1f) == 0x00 ) { _lenSP = 1; } // no quantization
  else                       { _lenSP = 2; }

  uint16_t n = (_size-3-_lenC)/_lenSP;
  for(uint16_t i=0; i<n; ++i)
  {
    _SP.push_back( get_field(fd,_lenSP,"SPqcc") );
  }
}

void QCC::display(std::ostream &s) const
{
  Xfrm_Sqcd sqcc_xfrm;  // Sqcd and Sqcc use same format

  Formatter out(s);
  out.display(_size, 2,     "Lqcc");
  out.display(_C,    _lenC, "Cqcc", "component index");
  out.display(_S,    1,     "Sqcc", sqcc_xfrm);

  Xfrm_SPqcd spqcc_xfrm(_S);  // SPqcd and SPqcc use same format

  std::string info = ( _lenSP == 1 
      ? "Exponent (epsilon_b)"
      : "Exponent (epsilon_b), Mantissa (mu_b)" );

  for(int i=0; i<_SP.size(); ++i)
  {
    out.display(_SP.at(i), _lenSP, "SPqcc", i, spqcc_xfrm, info); 
  }
}

