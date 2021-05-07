#ifndef _CODESTREAM_H_
#define _CODESTREAM_H_

#include "marker.h"
#include "fileheader.h"
#include "tilepart.h"

#include <iostream>
#include <vector>

class J2KCodestream
{
  public:
    typedef std::vector<J2KTilePart> TileParts_t;

  public:
    J2KCodestream(int fd);

    void display(std::ostream &s) const;

  private:
    J2KFileHeader _header;
    TileParts_t   _tiles;
    Marker        _eoc;
};

static std::ostream &operator<<(std::ostream &s,const J2KCodestream &x)
{
  x.display(s);
  return s;
}

#endif // _CODESTREAM_H_
