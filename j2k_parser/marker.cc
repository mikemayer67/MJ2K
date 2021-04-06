#include "marker.h"
#include "endian.h"
 
#include <unistd.h>
#include <stdint.h>

Marker::Marker(int fd)
{
  uint16_t code(0);
  int nread = read(fd, &code, 2);
  if(nread != 2) throw "Failed to read marker code";

#ifdef __LITTLE_ENDIAN__
  code = ((code & 0x00ff)<<8) | ((code & 0xff00)>>8);
#endif

  _code = static_cast<Code_t>(code);
}
