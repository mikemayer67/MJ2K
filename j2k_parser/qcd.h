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
    QCD(void)   {}
    QCD(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t Lqcd  (void) const { return _size; }
    uint8_t  Sqcd  (void) const { return _S; }

    uint16_t SPqcd(uint16_t i) const { return _SP.at(i); }

  private:
    uint8_t               _S;
    std::vector<uint16_t> _SP;
};

#endif // _QCD_H_
