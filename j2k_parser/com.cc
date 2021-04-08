#include "com.h"
#include "formatter.h"
#include "xfrm_com.h"

void COM::init(int fd)
{
  MarkerSegment::init(fd, Marker::COM );

  _R    = get_field(fd, 2, "Rcom");

  _C.read(fd,_size-4);
}

void COM::display(std::ostream &s) const
{
  Xfrm_Rcom rcom_xfrm;

  Formatter out(s);
  out.display(_size, 2, "Lcom");
  out.display(_R,    2, "Rcom", rcom_xfrm, "Comment content");

  if( binary() )
  {
    _C.dump(s);
  }
  else
  {
    s << "     ----------- " << std::endl
      << "     " << std::string(_C.data(), _C.end()) << std::endl
      << "     ----------- " << std::endl;
  }
}
