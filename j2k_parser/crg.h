#ifndef _CRG_H_
#define _CRG_H_

#include "segment.h"

#include <vector>

//------------------------------------------------------------------------------
// CRG - Component registration
//   Functional marker
//   File Header: optional
//   Tile Header: not allowed
//------------------------------------------------------------------------------

class CRG : public MarkerSegment
{
  public:
    CRG(void)   {}
    CRG(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t Lcrg(void) const { return _size; }

    uint16_t count(void) const { return _data.size(); }

    uint16_t Xcrg(uint16_t i) const { return _data.at(i).X; }
    uint32_t Ycrg(uint16_t i) const { return _data.at(i).Y; }

  private:

    struct Data_t
    {
      uint16_t X;
      uint16_t Y;
    };

    std::vector<Data_t> _data;
};

#endif // _CRG_H_
