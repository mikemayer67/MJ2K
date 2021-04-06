#ifndef _RGN_H_
#define _RGN_H_

#include "segment.h"
#include "siz.h"

//------------------------------------------------------------------------------
// RGN - Region of Inteest
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

class RGN : public MarkerSegment
{
  public:
    RGN(void)                   : MarkerSegment(Marker::RGN) {}
    RGN(int fd, const SIZ &siz) : MarkerSegment(Marker::RGN) { init(fd,siz); }

    void init(int fd, const SIZ &siz);
    void display(std::ostream &s) const;

  public:

    uint16_t Lrgn   (void) const { return _size; }
    uint16_t Crgn   (void) const { return _C;    }
    uint8_t  Srgn   (void) const { return _S;    }
    uint8_t  SPrgn  (void) const { return _SP;   }

  private:
    uint8_t  _lenC;

    uint16_t _C;
    uint8_t  _S;
    uint8_t  _SP;
};

#endif // _RGN_H_ 
