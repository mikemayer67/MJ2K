#ifndef _TLM_H_
#define _TLM_H_

#include "segment.h"

#include <vector>

//------------------------------------------------------------------------------
// TLM - Packet Length, main header
//   Functional marker
//   File Header: optional
//   Tile Header: not allowed
//------------------------------------------------------------------------------

class TLM : public MarkerSegment
{
  public:
    TLM(void)   {}
    TLM(int fd) { init(fd); }

    void init(int fd);
    void display(std::ostream &s) const;

    uint16_t      Ltlm(void) const { return _size; }
    uint8_t       Ztlm(void) const { return _Z; }
    uint8_t       Stlm(void) const { return _S; }

    bool hasTtlm(void) const { return _ST != 0; }

    uint16_t Ttlm(uint16_t i) const { return _T.at(i); }
    uint32_t Ptlm(uint16_t i) const { return _P.at(i); }

  private:
    uint8_t _Z;
    uint8_t _S;

    uint8_t _ST; // Ttlm size (bytes)
    uint8_t _SP; // Ptlm size (bytes)
    
    std::vector<uint16_t> _T;
    std::vector<uint32_t> _P;
};

#endif // _TLM_H_
