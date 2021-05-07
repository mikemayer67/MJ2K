#ifndef _RAWDATA_H_
#define _RAWDATA_H_

#include <iostream>
#include <stdint.h>

class RawData
{
  public:
    RawData(void)                   : _len(0), _data(NULL), _shareCounter(0) {}
    RawData(int fd,uint32_t nbytes) : _len(0), _data(NULL), _shareCounter(0) { read(fd,nbytes); }
    RawData(const RawData &ref)     : _len(0), _data(NULL), _shareCounter(0) { clone(ref);      }

    RawData &operator=(const RawData &ref) { clone(ref); return *this; }

    void read(int fd, uint32_t nbytes);  // Will replace current content

    ~RawData() { release(); }

    void release(void);
    void clone(const RawData &ref);

    void dump(std::ostream &s) const;

    uint32_t       length (void) const { return _len;  }
    const uint8_t *data   (void) const { return _data; }
    const uint8_t *end    (void) const { return _data + _len; }

  private:

    uint16_t *_shareCounter;

    uint32_t  _len;
    uint8_t  *_data;
};

static std::ostream &operator<<(std::ostream &s,const RawData &x) { x.dump(s); return s; }


#endif // _RAWDATA_H_
