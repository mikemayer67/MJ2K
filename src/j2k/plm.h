#ifndef _PLM_H_
#define _PLM_H_

#include "segment.h"
#include "rawdata.h"

#include <vector>

//------------------------------------------------------------------------------
// PLM - Packet Length, main header
//   Functional marker
//   File Header: optional
//   Tile Header: not allowed
//------------------------------------------------------------------------------

class PLM : public MarkerSegment
{
  public:
    PLM(void)   {}
    PLM(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    struct PacketLengths_t
    {
      uint8_t  Nplm;
      RawData  Iplm;
    };

    typedef std::vector<PacketLengths_t> Data_t;

    uint16_t      Lplm(void) const { return _size; }
    uint8_t       Zplm(void) const { return _Z; }
    const Data_t &data(void) const { return _data; }

    uint16_t count(void) const { return _data.size(); }

    uint16_t       Nplm(uint16_t i) const { return _data.at(i).Nplm; }
    const RawData &Iplm(uint16_t i) const { return _data.at(i).Iplm; }

  private:
    uint8_t _Z;
    Data_t  _data;
};

#endif // _PLM_H_
