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
