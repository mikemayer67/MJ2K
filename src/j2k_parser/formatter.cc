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

#include "formatter.h"

#include <sstream>
#include <string>
#include <iomanip>

using std::string;
using std::stringstream;
using std::setw;
using std::setfill;

#define LINE_INDENT  2
#define GAP_WIDTH    2
#define KEY_WIDTH   10
#define VALUE_WIDTH 12

Transformer Formatter::default_xform;

void Formatter::
display(uint32_t value, int nbytes, string key, int index, Transformer &xform, string info)
{
  if( nbytes == 0 ) return;

  if( index >= 0 )
  {
    stringstream ss;
    ss << key << '.' << 1+index;
    key = ss.str();
  }

  if( index > 0 )
  {
    info = "\"";
  }

  _s
    << string(LINE_INDENT,' ')
    << setw(KEY_WIDTH) << key
    << ":" << string(GAP_WIDTH-1,' ')
    << string(8-2*nbytes,'.')
    << setw(2*nbytes) << setfill('0') << std::hex << uint32_t(value) << setfill(' ')
    << string(GAP_WIDTH,' ');

  if(xform.wide())
  {
    _s 
      << setw(VALUE_WIDTH) << "----> "
      << string(GAP_WIDTH,' ')
      << xform(value);
  }
  else
  {
    _s
      << setw(VALUE_WIDTH) << xform(value) 
      << string(GAP_WIDTH,' ')
      << info;
  }

  _s <<std::endl;
}
