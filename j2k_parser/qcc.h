#ifndef _QCC_H_
#define _QCC_H_

#include "segment.h"
#include "siz.h"

#include <vector>

//------------------------------------------------------------------------------
// QCC - Quanitization Component
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

class QCC : public MarkerSegment
{
  public:
    QCC()                      : MarkerSegment(Marker::QCC) {}
    QCC(int fd,const SIZ &siz) : MarkerSegment(Marker::QCC) { init(fd,siz); }

    void init(int fd,const SIZ &siz);
    void display(std::ostream &s) const;

    uint16_t Lqcc  (void) const { return _size; }
    uint16_t Cqcc  (void) const { return _C; }
    uint8_t  Sqcc  (void) const { return _S; }

    uint16_t SPqcc(uint16_t i) const { 
      try { return _SP.at(i); }
      catch(std::out_of_range e) { throw_range_error("SPqcc",i,_SP.size()); }
    }

  private:
    uint8_t               _lenC;
    uint8_t               _lenSP;

    uint16_t              _C;
    uint8_t               _S;
    std::vector<uint16_t> _SP;
};

#endif // _QCC_H_
