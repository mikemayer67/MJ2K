#ifndef _QCD_H_
#define _QCD_H_

#include "segment.h"

#include <vector>

//------------------------------------------------------------------------------
// QCD - Quanitization Defaut
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

class QCD : public MarkerSegment
{
  public:
    QCD(void)   : MarkerSegment(Marker::QCD) {}
    QCD(int fd) : MarkerSegment(Marker::QCD) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t Lqcd  (void) const { return _size; }
    uint8_t  Sqcd  (void) const { return _S; }

    uint16_t SPqcd(uint16_t i) const { 
      try { return _SP.at(i); }
      catch(std::out_of_range e) { throw_range_error("SPqcd",i,_SP.size()); }
    }

  private:
    uint8_t               _S;
    std::vector<uint16_t> _SP;
};

#endif // _QCD_H_
