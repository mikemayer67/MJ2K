#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <bitset>
#include <chrono>

using namespace std;

bool trace = false;

typedef uint64_t   CardHash_t;
typedef bitset<4>  Suits_t;

typedef chrono::steady_clock Clock;
typedef Clock::time_point    TimePoint;

static const char *rankChar = "A23456789TJQK";
static const char *suitChar = "SHCD";

class Writable 
{
  public:
    virtual void write(ostream &s) const = 0;
};
ostream &operator<<(ostream &s, const Writable &x) { x.write(s); return s; }

class Hand : public Writable
{
  public:
    Hand(void) : _runHash(0), _setHash(0)
    {
      for(int i=0; i<10; ++i) {
        _deal[i] = i;
        flip(i);
      }
    }

    void flip(int n)
    {
      _runHash ^= (1UL << n);
      n = 4*(n%13) + (n/13); // switch from suit major to rank major ordering of bits
      _setHash ^= (1UL << n);
    }

    bool next(void)
    {
      if(_deal[0] == 42) return false;
      incr();
      return true;
    }

    CardHash_t runHash(void) const { return _runHash; }
    CardHash_t setHash(void) const { return _setHash; }

    void write(ostream &s) const {
      s << hex << setfill('0') << setw(16) << _runHash << "   " << dec;
      CardHash_t cards = _runHash;
      for(int suit=0; suit<4; ++suit) {
        if(suit>0) s << ' ';
        for(int rank=0; rank<13; ++rank) {
          if( cards & 0x1 ) { s << rankChar[rank]; }
          else                { s << '.'; }
          cards >>= 1;
        }
      }
    }

  private:

    uint8_t incr(int pos=9)
    {
      uint8_t card = _deal[pos];
      flip(card);

      if( card > 41+pos ) { card = 1 + incr(pos-1); }
      else                { card = 1 + card;        }

      _deal[pos] = card;
      flip(card);

      return card;
    }

    uint8_t    _deal[10];
    CardHash_t _runHash;
    CardHash_t _setHash;
};


class Set : public Writable
{
  public:
    Set(void) : _rank(0), _suits(0) {}

    void set(int rank, Suits_t suits)
    {
      _rank  = rank;
      _suits = suits;
    }

    void set(const Set &ref)
    {
      _rank  = ref._rank;
      _suits = ref._suits;
    }

    bool remove(int suit)
    {
      if( _suits.test(suit) == 0 ) return false;
      if( _suits.count() < 4     ) return false;
      _suits.flip(suit);
      return true;
    }

    int rank(void) const { return _rank; }

    int ncards(void) const { return int(_suits.count()); }

    bool contains(int suit) const { return _suits[suit]; }

    void write(ostream &s) const { s << "S" << ncards(); }

    string details(void) const {
      stringstream rval;
      rval << "[" << rankChar[_rank] << " ";
      for(int suit=0; suit<4; ++suit ) {
        if(_suits[suit]) rval << suitChar[suit];
      }
      rval << "]";
      return rval.str();
    }

  private:
    int     _rank;
    Suits_t _suits;
};

class Sets : public Writable
{
  public:
    Sets(const Hand &hand) : _nsets(0)
    {
      static const CardHash_t setMask = 0x0f;

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

    string details(void) const {
      stringstream rval;
      for(int i=0; i<_nsets; ++i) rval << _sets[i].details();
      return rval.str();
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

  string details(void) const { 
    stringstream rval;
    rval << "[" << rankChar[_start] << "-" << rankChar[_end-1] << " " << suitChar[_suit] << "]";
    return rval.str();
  }

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

    string details(void) const
    {
      stringstream rval;
      for(int i=0; i<_nruns; ++i) rval << _runs[i].details();
      return rval.str();
    }

  private:
    int _nruns;
    Run _runs[3];
};


struct Overlap
{
  int rank;
  int suit;
};


class Overlaps : public Writable
{
  public:
    Overlaps(const Runs &runs, const Sets &sets) : _count(0)
    {
      for(int i=0; i<runs.count(); ++i) {
        int suit = runs[i].suit();
        for(int j=0; j<sets.count(); ++j) {
          if( sets[j].contains(suit) ) {
            int rank = sets[j].rank();
            if( runs[i].contains(rank) ) {
              _overlap[_count].rank = rank;
              _overlap[_count].suit = suit;
              ++_count;
            }
          }
        }
      }
    }

    int ncards(void) const { return _count; }

    const Overlap &operator[](int n) const 
    {
      assert(n>=0);
      assert(n<_count);
      return _overlap[n];
    }

    void write(ostream &s) const 
    {
      char delim = ':';
      s << "O";
      for(int i=0; i<_count; ++i)
      {
        s << delim << rankChar[_overlap[i].rank] << suitChar[_overlap[i].suit];
        delim = '.';
      }
    }

    bool is_gin(const Runs &runs, const Sets &sets, int index=0) const
    {
      int rank = _overlap[index].rank;
      int suit = _overlap[index].suit;

      if(trace) {
        if(index == 0) cout << runs.details() << " " << sets.details() << endl;
        cout << index << " " << rankChar[rank] << suitChar[suit] << endl;
      }

      for(int i=0; i<runs.count(); ++i) {
        Runs newRuns(runs);
        if( newRuns.remove(i,rank) ) {
          if( index == _count-1 ) return true;
          if( is_gin( newRuns, sets, index+1) ) return true;
        }
      }

      for(int i=0; i<sets.count(); ++i) {
        Sets newSets(sets);
        if( newSets.remove(i,suit) ) {
          if( index == _count-1 ) return true;
          if( is_gin( runs, newSets, index+1) ) return true;
        }
      }

      return false;
    }

  private:
    int     _count;
    Overlap _overlap[9];
};


int main(int argc, char **argv)
{
  TimePoint startTime = Clock::now();

  uint64_t nhands = 0;
  uint64_t ngin   = 0;

  double ntotal = 1.0;
  for(int i=43; i<=52; ++i) { ntotal *= i; }
  for(int i= 2; i<=10; ++i) { ntotal /= i; }

  Hand hand;
  do {
    ++nhands;

    uint64_t traceHash = (0x3fUL | (0x30UL<<13) | (0x30UL<<26)) ;

    trace = traceHash == hand.runHash();

//    if( nhands % 100000000 == 0) {
//      TimePoint now = Clock::now();
//
//      cout << hand << "   " 
//        << fixed << setw(8) << setprecision(3)
//        << 0.001 * chrono::duration_cast<chrono::milliseconds>(now-startTime).count() 
//        << "   " << ngin << " / " 
//        << fixed << setw(5) << setprecision(1) << double(100.0*nhands)/ntotal << "%"
//        << endl;
//    }

    Runs runs(hand);

    if(runs.ncards() == 10) {
      cout << hand << "   : " << runs << endl; 
      ++ngin;
      continue;
    }

    Sets sets(hand);

    if(sets.ncards() == 10) {
      cout << hand << "   : " << sets << endl; 
      ++ngin;
      continue;
    }

    Overlaps overlaps(runs,sets);

    if( trace )
    {
      cout << hand << "  : " << runs << " " << sets << endl;
      cout << runs.ncards() << " " << sets.ncards() << " " << overlaps.ncards() << endl;
    }

    if( (runs.ncards() + sets.ncards() - overlaps.ncards() < 10)) 
    {
      if(trace) { cout << "nope..." << endl; exit(1); }
      continue;
    }

    if( overlaps.ncards() == 0 )  {
      cout << hex << setw(32) << hand.runHash() << dec << " " << hand << "   : " << runs << "  " << sets << endl;
      ++ngin;
      if(trace) { cout << "gin: no overlap.." << endl; exit(1); }
      continue;
    }
    
    if( overlaps.is_gin(runs, sets) ) {
      cout << hex << setw(32) << hand.runHash() << dec << " " << hand << "   : " << runs << "  " << sets << endl;
      ++ngin;
      if(trace) { cout << "gin: with overlap.." << endl; exit(1); }
      continue;
    }
    if(trace) { cout << "no gin: " << endl; exit(1); }

  } while (hand.next());

  TimePoint endTime = Clock::now();

  cout << endl 
    << "Examined: " << nhands << endl
    << "     Gin: " << ngin   << endl
    << endl
    << fixed << setw(8) << setprecision(3)
    << 0.001 * chrono::duration_cast<chrono::milliseconds>(endTime-startTime).count() 
    << endl;

  return 0;
}

