#ifndef _PLT_H_
#define _PLT_H_

#include "segment.h"
#include "rawdata.h"

//------------------------------------------------------------------------------
// PLT - Packet Length, tile-part
//   Functional marker
//   File Header: optional
//   Tile Header: not allowed
//------------------------------------------------------------------------------

class PLT : public MarkerSegment
{
  public:
    PLT(void)   {}
    PLT(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t       Lplt(void) const { return _size; }
    uint8_t        Zplt(void) const { return _Z; }
    const RawData &Iplt(void) const { return _I; }

  private:
    uint8_t _Z;
    RawData _I;
};

#endif // _PLT_H_
