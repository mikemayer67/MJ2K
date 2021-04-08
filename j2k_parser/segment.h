#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "marker.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <stdint.h>
#include <cassert>

// Abstract base class for all marker segments

class MarkerSegment
{
  public:

    MarkerSegment(void) : _size(0) {}

    virtual void init(int fd, Marker::Code_t);
    virtual void display(std::ostream &s) const {}

    const Marker &code(void) const { return _marker; }
    uint16_t      size(void) const { return _size; }

    void head(std::ostream &s) const;

  protected:

    uint32_t get_field(int fd, int nbytes, std::string field) const;

    void throw_range_error(std::string field, uint16_t index, uint16_t array_size) const;

  protected:
    Marker   _marker;
    uint16_t _size;
};

static std::ostream &operator<<(std::ostream &s, const MarkerSegment &x) 
{ 
  x.head(s);
  x.display(s); 
  return s; 
}

#endif // _SEGMENT_H_
