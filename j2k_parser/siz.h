#ifndef _SIZ_H_
#define _SIZ_H_

#include "segment.h"

#include <vector>

//------------------------------------------------------------------------------
// SIZ - Image and tile size
//   Fixed info marker
//   File Header: required (must immediately follow SOC)
//   Tile Header: not allowed
//------------------------------------------------------------------------------

class SIZ : public MarkerSegment
{
  public:
    SIZ(void)   {}
    SIZ(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t Lsiz   (void) const { return _size; }
    uint16_t Rsiz   (void) const { return _R;   }
    uint32_t Xsiz   (void) const { return _X;   }
    uint32_t Ysiz   (void) const { return _Y;   }
    uint32_t XOsiz  (void) const { return _XO;  }
    uint32_t YOsiz  (void) const { return _YO;  }
    uint32_t XTsiz  (void) const { return _XT;  }
    uint32_t YTsiz  (void) const { return _YT;  }
    uint32_t XTOsiz (void) const { return _XTO; }
    uint32_t YTOsiz (void) const { return _YTO; }
    uint16_t Csiz   (void) const { return _C;   }

    uint8_t Ssiz  (uint16_t i) const { return _compData.at(i).S; }
    uint8_t XRSiz (uint16_t i) const { return _compData.at(i).XR; }
    uint8_t YRsiz (uint16_t i) const { return _compData.at(i).YR; }
  private:
    uint16_t _R;
    uint32_t _X;
    uint32_t _Y;
    uint32_t _XO;
    uint32_t _YO;
    uint32_t _XT;
    uint32_t _YT;
    uint32_t _XTO;
    uint32_t _YTO;
    uint16_t _C;

    struct CompData
    {
      uint8_t S;
      uint8_t XR;
      uint8_t YR;
    };

    std::vector<CompData> _compData;
};

#endif // _SIZ_H_ 
