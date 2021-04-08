#ifndef _TILEPART_H_
#define _TILEPART_H_

#include "marker.h"
#include "siz.h"
#include "sot.h"

#include <iostream>
#include <vector>

class J2KTilePart
{
  public:
    J2KTilePart(void)   {};
    J2KTilePart(int fd, const SIZ &siz) { init(fd,siz); }

    void init(int fd, const SIZ &siz);
    void display(std::ostream &s) const;

  private:
    SOT    _sot;
    Marker _sod;

    std::vector<MarkerSegment*> _segments;
};

static std::ostream &operator<<(std::ostream &s, const J2KTilePart &x) 
{ 
  x.display(s); 
  return s; 
}

#endif // _TILEPART_H_
