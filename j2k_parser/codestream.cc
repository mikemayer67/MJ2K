#include "codestream.h"

#include "marker.h"

J2KCodestream::J2KCodestream(int fd)
{
  _header.init(fd);
}

void J2KCodestream::display(std::ostream &s) const
{
  s << _header;
}
