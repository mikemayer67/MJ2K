#ifndef _SOT_H_
#define _SOT_H_

#include "segment.h"

//------------------------------------------------------------------------------
// SOT - Start of tile-part
//   Fixed info marker
//   File Header: not allowed
//   Tile Header: required (first marker in tile-part)
//------------------------------------------------------------------------------

class SOT : public MarkerSegment
{
  public:
    SOT(void)   {}
    SOT(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t Lsot  (void) const { return _size; }
    uint16_t Isot  (void) const { return _I;    } 
    uint32_t Psot  (void) const { return _P;    } 
    uint8_t  TPsot (void) const { return _TP;   } 
    uint8_t  TNsot (void) const { return _TN;   }

    uint64_t end_of_data(void) const { return _marker.offset() + _P; }

  private:
    uint16_t _I;
    uint32_t _P;
    uint8_t  _TP;
    uint8_t  _TN;
};


#endif // _SOT_H_
