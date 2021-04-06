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
