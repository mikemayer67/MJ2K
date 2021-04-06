#ifndef _COM_H_
#define _COM_H_

#include "segment.h"

#include <vector>

//------------------------------------------------------------------------------
// COM - Image and tile come
//   Fixed info marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

class COM : public MarkerSegment
{
  public:
    COM(void)   : MarkerSegment(Marker::COM) {}
    COM(int fd) : MarkerSegment(Marker::COM) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t Lcom (void) const { return _size; }
    uint16_t Rcom (void) const { return _R;   }

    std::vector<uint8_t> Ccom (void) const { return _C;  }

    bool binary(void) const { return _R == 0x00; }

  private:
    uint16_t             _R;
    std::vector<uint8_t> _C;
};

#endif // _COM_H_ 
