#include "com.h"
#include "formatter.h"
#include "xfrm_com.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <string.h>

void COM::init(int fd)
{
  _size = get_field(fd, 2, "Lcom");
  _R    = get_field(fd, 2, "Rcom");

  int len = _size-4;
  uint8_t *buffer = new uint8_t[len];

  int nread = read(fd, buffer, len);
  if(nread != len)
  {
    std::stringstream err;
    err << "Failed to read data for Ccom field (file offset: "
      << lseek(fd,0,SEEK_CUR) << ") : " <<strerror(errno);
    throw err.str();
  }

  _C.insert(_C.end(), buffer, buffer+len);

  delete[] buffer;
}

void COM::display(std::ostream &s) const
{
  s << "COM" << std::endl;

  Xfrm_Rcom rcom_xfrm;

  Formatter out(s);
  out.display(_size, 2, "Lcom");
  out.display(_R,    2, "Rcom", rcom_xfrm, "Comment content");

  if( binary() )
  {
    s << "----------- " << std::endl;

    uint16_t n(0);
    for(std::vector<uint8_t>::const_iterator b=_C.begin(); b!=_C.end(); ++b, ++n)
    {
      uint16_t v = *b;
      s << std::setw(2) << std::setfill('0') << std::hex << v << " ";
      if( n%32 == 31 )    { s << std::endl; }
      else if (n%8 == 7 ) { s << " "; }
    }
    if( n%32 ) { s << std::endl; }

    s << "----------- " << std::endl;
  }
  else
  {
    s << "----------- " << std::endl
      << std::string(_C.begin(), _C.end()) << std::endl
      << "----------- " << std::endl;
  }
}
