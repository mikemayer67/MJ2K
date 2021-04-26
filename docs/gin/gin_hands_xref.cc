#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>

#include <stdlib.h>
#include <cstring>

using namespace std;

typedef bitset<52> Hand_t;

istream &operator>>(istream &s,string &str) { return getline(s,str); }


class Writable 
{
  public:
    virtual void write(ostream &s) const = 0;
};
ostream &operator<<(ostream &s, const Writable &x) { x.write(s); return s; }

class Set : public Writable
{
  public:
    Set(void) : _rank(0), _suits(0) {}

    void set(int rank, Suits_t suits)
    {
      _rank  = rank;
      _suits = suits;
    }

    int rank(void) const { return _rank; }
    int ncards(void) const { return int(_suits.count()); }
    bool contains(int suit) const { return _suits[suit]; }
    void write(ostream &s) const { s << "S" << ncards(); }

  private:
    int     _rank;
    Suits_t _suits;
};

class Sets : public Writable
{
  public:
    Sets(const Hand_t &hand) : _nsets(0)
    {
      static const CardHash_t setMask = 0x0f;

      Hand_t 
      CardHash_t setHash = hand.setHash();

      for(int rank=0; rank<13; ++rank)
      {
        Suits_t suits = uint8_t(setHash & setMask);
        if( suits.count() > 2 )
        {
          _sets[_nsets].set(rank,suits);
          ++_nsets;
        }
        setHash >>= 4;
      }
    }

    Sets(const Sets &ref) : _nsets(ref._nsets)
    {
      for(int i=0; i<_nsets; ++i) _sets[i].set(ref._sets[i]);
    }

    bool remove(int i, int suit)
    {
      return _sets[i].remove(suit);
    }

    const Set &operator[](int n) const
    {
      assert(n>=0);
      assert(n<_nsets);
      return _sets[n];
    }

    int count(void)  const { return _nsets; }

    int ncards(void) const { 
      int rval = 0;
      for(int i=0; i<_nsets; ++i) { rval += _sets[i].ncards(); }
      return rval;
    }

    void write(ostream &s) const
    {
      for(int i=0; i<_nsets; ++i)
      {
        if(i>0) s << ' ';
        s << _sets[i];
      }
    }

  private:
    int _nsets;
    Set _sets[3];
};


class Run : public Writable
{
  public:
  Run(void) : _suit(0), _start(0), _end(0) {}

  void set(int suit, int start, int end) {
    _suit  = suit;
    _start = start;
    _end   = end;
  }

  void set(const Run &ref)
  {
    _suit  = ref._suit;
    _start = ref._start;
    _end   = ref._end;
  }

  bool remove(int rank)
  {
    if( _end < _start + 4 ) return false;
    if( rank == _start ) {
      _start += 1;
      return true;
    }
    if( rank == _end - 1 ) {
      _end -= 1;
      return true;
    }
    return false;
  }

  bool split(int rank, Run &into)
  {
    if( _end < _start + 7 ) return false;
    if( rank < _start + 3 ) return false;
    if( rank > _end   - 4 ) return false;
    into.set(_suit, rank+1, _end);
    _end = rank;
    return true;
  }

  int suit(void) const { return _suit; }

  int ncards(void) const { return _end - _start; }

  bool contains(int rank) const { return ((rank >= _start) && (rank < _end)); }

  void write(ostream &s) const { s << "R" << ncards(); }

  private:
  int _suit;
  int _start;
  int _end;
};

class Runs : public Writable
{
  public:
    Runs(const Hand &hand) : _nruns(0)
    {
      static const CardHash_t runMask = 0x07;

      CardHash_t runHash = hand.runHash();

      for(int suit=0; suit<4 && runHash != 0; ++suit)
      {
        int start = -1;
        for(int rank=0; rank<11 && runHash != 0; ++rank) // tail of last possible run is JQK (aka 10,11,12)
        {
          if( (runHash & runMask) == runMask ) { // in a run
            if( start < 0 ) start = rank;
          }
          else if( start >= 0 ) { // found end of run
            // ....xxxxx....
            //    ^ ^  ^
            //    | |  start
            //    | rank
            //    end
            _runs[_nruns].set(suit,start,rank+2);
            ++_nruns;

            start = -1;
          }
          runHash >>= 1;
        }
        if( start >= 0 ) { // run ran to end of suit
          _runs[_nruns].set(suit,start,13);
          ++_nruns;
        }
        runHash >>= 2; // remember, we skipped ranks 11 and 12
      }
    }

    Runs(const Runs &ref) : _nruns(ref._nruns)
    {
      for(int i=0; i<_nruns; ++i)  _runs[i].set(ref._runs[i]);
    }

    bool remove(int i, int rank)
    {
      if( _runs[i].remove(rank) ) return true;
      if( _nruns == 3 ) return false;
      if( _runs[i].split(rank, _runs[_nruns]) ) {
        ++_nruns;
        return true;
      }
      return false;
    }


    const Run &operator[](int n) const
    {
      assert(n>=0);
      assert(n<_nruns);
      return _runs[n];
    }

    int count(void)  const { return _nruns; }

    int ncards(void) const { 
      int rval = 0;
      for(int i=0; i<_nruns; ++i) { rval += _runs[i].ncards(); }
      return rval;
    }

    void write(ostream &s) const
    {
      for(int i=0; i<_nruns; ++i)
      {
        if(i>0) s << ' ';
        s << _runs[i];
      }
    }

  private:
    int _nruns;
    Run _runs[3];
};

int main(int argc,const char **argv)
{
  if(argc!=2) {
    cerr << "Usage:: " << argv[0] << " hands_data_file" << endl;
    exit(1);
  }

  ifstream hands_file(argv[1]);
  if(!hands_file) {
    cerr << "Sorry:: Failed to open: " << argv[1] << ": " << strerror(errno) << endl;
    exit(1);
  }
  
  string line;
  bitset<52> hand = 0;
  while(true)
  {
    string line;
    hands_file >> line;

    if(!hands_file) break;

    if(line.length() < 16) continue;
    if(line.find_first_not_of("0123456789abcdef",0)<16) continue;
    line = line.substr(0,16);

    hand = stoul(line,0,16);

    cout << hand << endl;
  }

}
