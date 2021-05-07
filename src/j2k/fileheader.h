#ifndef _FILEHEADER_H_
#define _FILEHEADER_H_

#include "marker.h"
#include "siz.h"

#include <iostream>

class J2KFileHeader
{
  public:
    J2KFileHeader(void)   {}
    J2KFileHeader(int fd) { init(fd); }

    void init(int fd);

    void display(std::ostream &s) const;

    const SIZ &siz(void) const { return _siz; }

  private:
    Marker _soc;
    SIZ    _siz;

    std::vector<MarkerSegment*> _segments;
};

static std::ostream &operator<<(std::ostream &s, const J2KFileHeader &x) 
{ 
  x.display(s); 
  return s; 
}

#endif // _FILEHEADER_H_

