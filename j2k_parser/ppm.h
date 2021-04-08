#ifndef _PPM_H_
#define _PPM_H_

#include "segment.h"
#include "rawdata.h"

#include <vector>

//------------------------------------------------------------------------------
// PPM - Packed packet headers, main header
//   Functional marker
//   File Header: optional
//   Tile Header: not allowed
//------------------------------------------------------------------------------

class PPM : public MarkerSegment
{
  public:
    PPM(void)   {}
    PPM(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    struct PacketHeaders_t
    {
      uint32_t Nppm;
      RawData  Ippm;
    };

    typedef std::vector<PacketHeaders_t> Data_t;

    uint16_t      Lppm(void) const { return _size; }
    uint8_t       Zppm(void) const { return _Z; }
    const Data_t &data(void) const { return _data; }

    uint16_t count(void) const { return _data.size(); }

    uint16_t        Nppm(uint16_t i) const { return _data.at(i).Nppm; }
    const RawData  &Ippm(uint16_t i) const { return _data.at(i).Ippm; }

  private:
    uint8_t _Z;
    Data_t  _data;
};

#endif // _PPM_H_
