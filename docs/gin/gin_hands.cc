#include <iostream>
#include <iomanip>
#include <bitset>

using namespace std;

class Counts
{
  public:
    Counts(void) { memset(this,0,sizeof(Counts)); }

    void hand       (void)  { _hands++;                 }
    void three_sets (void)  { _gin++; _scenario[6]++;   }
    void runs       (int n) { _gin++; _scenario[n-1]++; }

    void dump(ostream &s) const {
      s << _gin << ": " << _scenario[0];
      for(int i=1; i<7; ++i) { s << " " << _scenario[i]; }
    }

    void summary(ostream &s) const { 
      s << "----------------------------------" << endl
        << "    hands       : " << _hands << endl
        << "      gin       : " << _gin << endl
        << " 1) 1 Run       : " << _scenario[0] << endl
        << " 2) 2 Runs      : " << _scenario[1] << endl
        << " 3) 3 Sets      : " << _scenario[2] << endl
        << " 4) 1 Run/1Set  : " << _scenario[3] << endl
        << " 5) 2 Runs/1Set : " << _scenario[4] << endl
        << " 6) 1 Run/2Sets : " << _scenario[5] << endl
        << " 7) 3 Sets      : " << _scenario[6] << endl
        << "----------------------------------" << endl
        ;
    }

    uint64_t hands(void) const { return _hands; }

  private:
    uint64_t _hands;
    uint64_t _gin;
    uint64_t _scenario[7];
};
ostream &operator<<(ostream &s, const Counts &x) { x.dump(s); return s; }

typedef bitset<52> Cards_t;
typedef bitset<4>  Suits_t;

class Set
{
  public:
    Set(void) { _suits.reset(); }

    void set(int rank, Suits_t suits)
    {
      _rank   = rank;
      _suits  = suits;
    }

    int length(void) const { return _suits.count(); }
    int rank(void)   const { return _rank; }

    bool contains(int suit) const { return _suits[suit]; }

  private:
    int     _rank;
    Suits_t _suits;
};

class Sets
{
  public:
    Sets(Cards_t cards) : _nsets(0), _ncards(0)
    {
      Suits_t suits;
      for(int rank=0; rank<13; ++rank)
      {
        suits.reset();
        for(int suit=0; suit<4; ++suit)
        {
          if( cards[13*suit+rank] ) suits.flip(suit);
        }
        int n = suits.count();
        if(n >= 3)
        {
          assert(_nsets < 3);
          _ncards += n;
          _sets[_nsets++].set(rank,suits);
        }
      }
    }

    int count  (void) const { return _nsets;  }
    int ncards (void) const { return _ncards; }

    const Set &operator[](int i) const { 
      assert(i < _nsets);
      return _sets[i];
    }


  private:
    int _ncards;
    int _nsets;
    Set _sets[3];
};

class Run
{
  public:
    Run(void) : _suit(0), _start(0), _end(0) {}

    void set(int suit, int start, int end)
    {
      _suit   = suit;
      _start  = start;
      _end    = end;
    }

    int suit  (void) const { return _suit; }
    int length(void) const { return _end - _start; }

  private:
    int _suit;
    int _start;
    int _end;
};

class Runs
{
  public:
    Runs(Cards_t cards) : _nruns(0), _ncards(0)
    {
      int card = 0 ;
      for(int suit=0; suit<4; ++suit)
      {
        int start = 0;
        for(int rank=0; rank<13; ++rank, ++card)
        {
          if( ! cards[card] )
          {
            // outside run
            if( start < rank - 2 )
            {
              assert(_nruns<3);
              _ncards += rank-start;
              _runs[_nruns++].set(suit,start,rank);
            }
            start = rank + 1;
          }
        }
        if( start < 11 )
        {
          assert(_nruns<3);
          _ncards += 13-start;
          _runs[_nruns++].set(suit,start,13);
        }
      }
    }

    int count  (void) const { return _nruns;  }
    int ncards (void) const { return _ncards; }

    const Run &operator[](int i) const { 
      assert(i < _nruns);
      return _runs[i];
    }

  private:
    int _ncards;
    int _nruns;
    Run _runs[3];
};

class Overlap
{
  public:
    Overlap(Runs &runs, Sets &sets) : _ncards(0)
    {
      for(int i=0; i<runs.count(); ++i)
      {
        int suit = runs[i].suit();
        for(int j=0; j<sets.count(); ++j)
        {
          if( sets[j].contains(suit) )
          {
            _ranks[_ncards] = sets[j].rank();
            _suits[_ncards] = suit;
            ++_ncards;
          }
        }
      }
    }

    int ncards (void) const { return _ncards; }

  private:
    int _ncards;
    int _ranks[9];
    int _suits[9];
};


class Hand
{
  public:
    Hand(void) : done(false)
    {
      cards.reset();
      for(int i=0; i<10; ++i) {
        deal[i] = i;
        cards.flip(i);
      }
    }

    operator bool() const { return !done; }

    Hand &operator++() 
    {
      if(deal[0] < 42) { incr(); }
      else             { done = true; }
      return *this;
    }

    void write(ostream &s) const {
      for(int bit=0; bit<52; ++bit) {
        if(bit % 13 == 0) s << ' ';
        s << cards[bit];
      }
    }

    void evaluate(Counts &counts) const;

  private:

    uint8_t incr(int pos=9)
    {
      assert(pos>=0);

      uint8_t card = deal[pos];
      cards.flip(card);

      if( card > 41+pos ) { card = 1 + incr(pos-1); }
      else                { card = 1 + card;        }

      deal[pos] = card;
      cards.flip(card);

      return card;
    }


    bool    done;
    uint8_t deal[10];
    Cards_t cards;
};
ostream &operator<<(ostream &s, const Hand &x) { x.write(s); return s; }



int main(int argc, char **argv)
{
  Counts counts;
  for(Hand hand; hand; ++hand) hand.evaluate(counts);
  counts.summary(cout);
  return 0;
}



void Hand::evaluate(Counts &counts) const
{
  counts.hand();
  if(counts.hands() % 100000000 == 0) { write(cout); cout << endl; }

  Sets sets(cards);
  if( sets.ncards() == 10 )
  {
    // all cards are in a set
    counts.three_sets();
    return;
  }

  Runs runs(cards);
  if( runs.ncards() == 10 )
  {
    // all cards are in a run
    counts.runs(runs.count());
    cout << *this << "  " << counts << endl;
    return;
  }

  Overlap overlap(runs,sets);

  if( sets.ncards() + runs.ncards() - overlap.ncards() )
  {
    // at least one card is not in a run or a set... no gin
    return;
  }

}

