#ifndef _COM_H_
#define _COM_H_

#include "segment.h"
#include "rawdata.h"

//------------------------------------------------------------------------------
// COM - Image and tile come
//   Fixed info marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

class COM : public MarkerSegment
{
  public:
    COM(void)   {}
    COM(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t Lcom (void) const { return _size; }
    uint16_t Rcom (void) const { return _R;   }

    const RawData &Ccom (void) const { return _C;  }

    bool binary(void) const { return _R == 0x00; }

  private:
    uint16_t _R;
    RawData  _C;
};

#endif // _COM_H_ 
