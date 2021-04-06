#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "marker.h"

#include <string>
#include <stdint.h>
#include <stdexcept>

// Abstract base class for all marker segments

class MarkerSegment
{
  public:

    MarkerSegment(Marker::Code_t code) : _code(code), _size(0) {}

    virtual void display(std::ostream &s) const {}

    Marker::Code_t code(void) const { return _code; }
    uint16_t       size(void) const { return _size; }

    bool initialized(void) const { return _size > 0; }

  protected:

    uint32_t get_field(int fd, int nbytes, std::string field) const;

    void throw_range_error(std::string field, uint16_t index, uint16_t array_size) const;

  protected:
    Marker::Code_t _code;
    uint16_t       _size;
};

static std::ostream &operator<<(std::ostream &s, const MarkerSegment &x) 
{ 
  x.display(s); 
  return s; 
}

#endif // _SEGMENT_H_
