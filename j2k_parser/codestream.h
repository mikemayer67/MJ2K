#ifndef _CODESTREAM_H_
#define _CODESTREAM_H_

#include "fileheader.h"

#include <iostream>

class J2KCodestream
{
  public:
    J2KCodestream(int fd);

    void display(std::ostream &s) const;

  private:
    J2KFileHeader _header;
};

static std::ostream &operator<<(std::ostream &s,const J2KCodestream &x)
{
  x.display(s);
  return s;
}

#endif // _CODESTREAM_H_
