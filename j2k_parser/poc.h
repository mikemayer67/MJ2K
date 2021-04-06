#ifndef _POC_H_
#define _POC_H_

#include "segment.h"
#include "siz.h"

#include <vector>

//------------------------------------------------------------------------------
// POC - Region of Inteest
//   Functional marker
//   File Header: optional
//   Tile Header: optional
//------------------------------------------------------------------------------

class POC : public MarkerSegment
{
  public:
    POC(void)                   : MarkerSegment(Marker::POC) {}
    POC(int fd, const SIZ &siz) : MarkerSegment(Marker::POC) { init(fd,siz); }

    void init(int fd, const SIZ &siz);
    void display(std::ostream &s) const;

  public:

    struct ProgOrderChange_t
    {
      uint8_t  RS;
      uint16_t CS;
      uint16_t LYE;
      uint8_t  RE;
      uint8_t  S;
      uint16_t CE;
      uint8_t  P;
    };

    uint16_t Lrgn   (void) const { return _size; }

    uint8_t RSpoc(uint16_t i) const { 
      try { return _progOrderChange.at(i).RS; }
      catch(std::out_of_range e) { throw_range_error("RSpoc",i,_progOrderChange.size()); }
    }

    uint16_t CSpoc(uint16_t i) const { 
      try { return _progOrderChange.at(i).CS; }
      catch(std::out_of_range e) { throw_range_error("CSpoc",i,_progOrderChange.size()); }
    }

    uint16_t LYEpoc(uint16_t i) const { 
      try { return _progOrderChange.at(i).LYE; }
      catch(std::out_of_range e) { throw_range_error("LYEpoc",i,_progOrderChange.size()); }
    }

    uint8_t REpoc(uint16_t i) const { 
      try { return _progOrderChange.at(i).RE; }
      catch(std::out_of_range e) { throw_range_error("REpoc",i,_progOrderChange.size()); }
    }

    uint16_t CEpoc(uint16_t i) const { 
      try { return _progOrderChange.at(i).CE; }
      catch(std::out_of_range e) { throw_range_error("CEpoc",i,_progOrderChange.size()); }
    }

    uint8_t Ppoc(uint16_t i) const { 
      try { return _progOrderChange.at(i).P; }
      catch(std::out_of_range e) { throw_range_error("Ppoc",i,_progOrderChange.size()); }
    }

  private:
    uint8_t  _lenC;

    std::vector<ProgOrderChange_t> _progOrderChange;
};

#endif // _POC_H_ 
