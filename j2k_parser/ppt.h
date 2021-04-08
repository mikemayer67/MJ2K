#ifndef _PPT_H_
#define _PPT_H_

#include "segment.h"
#include "rawdata.h"

//------------------------------------------------------------------------------
// PPT - Packed packet headers, tile-part
//   Functional marker
//   File Header: not allowed
//   Tile Header: optional
//------------------------------------------------------------------------------

class PPT : public MarkerSegment
{
  public:
    PPT(void)   {}
    PPT(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t       Lppt(void) const { return _size; }
    uint8_t        Zppt(void) const { return _Z; }
    const RawData &Ippt(void) const { return _I; }

  private:
    uint8_t _Z;
    RawData _I;
};

#endif // _PPT_H_
