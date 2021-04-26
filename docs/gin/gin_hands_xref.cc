#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stdlib.h>
#include <bitset>
#include <cassert>
#include <cstring>

using namespace std;

bool trace = false;

typedef bitset<52> Hand_t;
typedef bitset<4>  Suits_t;

const string rankChar = "A23456789TJQK";
const string suitChar = "SHCD";

istream &operator>>(istream &s,string &str) { return getline(s,str); }

class Writable 
{
  public:
    virtual void write(ostream &s) const = 0;
};
ostream &operator<<(ostream &s, const Writable &x) { x.write(s); return s; }

class Hand : public Writable
{
  public:
    Hand(uint64_t hand) : _hand(hand) 
    {
//      trace = hand == ( 0x7fUL | (0x01UL<<13) | (0x01UL<<26) | (0x01UL<<39) );
    }

    bool test(int rank,int suit) const { return _hand.test(13*suit + rank); }

    void write(ostream &s) const {
      for(int suit=0; suit<4; ++suit) {
        if(suit > 0) s << " ";
        for(int rank=0; rank<13; ++rank) s << (test(rank,suit) ? rankChar[rank] : '.');
      }
    }

  private:
    Hand_t _hand;
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

    int rank(void) const { return _rank; }
    int ncards(void) const { return int(_suits.count()); }
    bool contains(int suit) const { return _suits.test(suit); }
    void write(ostream &s) const { s << "S" << ncards(); }

    string details(void) const {
      stringstream s;
      s << "(" << _rank << "|";
      for(int suit=0; suit<4; ++suit) {
        if(_suits.test(suit)) s << suitChar[suit];
      }
      s << ")";
      return s.str();
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
      for(int rank=0; rank<13; ++rank)
      {
        Suits_t suits = 0;
        for(int suit=0; suit<4; ++suit) {
          if( hand.test(rank,suit) ) suits.flip(suit);
        }
        if( suits.count() > 2 )
        {
          _sets[_nsets].set(rank,suits);
          ++_nsets;
        }
      }

      if(trace) cout << "Sets: " << details() << endl;
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

    string details(void) const
    {
      string rval;
      for(int i=0; i<_nsets; ++i) {
        rval += _sets[i].details();
      }
      return rval;
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

    int suit(void) const { return _suit; }
    int ncards(void) const { return _end - _start; }
    bool contains(int rank) const { return ((rank >= _start) && (rank < _end)); }
    void write(ostream &s) const { s << "R" << ncards(); }

    string details(void) const {
      stringstream rval;
      rval << "(" << _suit << "|" << _start << "-" << _end << ")"; 
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
      for(int suit=0; suit<4; ++suit)
      {
        int start = -1;
        for(int rank=0; rank<11; ++rank)
        {
          if( hand.test(rank,suit) && hand.test(rank+1,suit) && hand.test(rank+2,suit) )
          {
            if( start < 0 ) start = rank;
          }
          else if( start >= 0 ) {
            _runs[_nruns].set(suit,start,rank+2);
            ++_nruns;

            start = -1;
          }
        }
        if( start >= 0 ) {
          _runs[_nruns].set(suit,start,13);
          ++_nruns;
        }
      }

      if(trace) cout << "Runs: " << details() << endl;
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

    int nsuits(void) const {
      Suits_t suits = 0;
      for(int i=0; i<_nruns; ++i) suits.set(_runs[i].suit());
      return suits.count();
    }

    string details(void) const
    {
      string rval;
      for(int i=0; i<_nruns; ++i) {
        rval += _runs[i].details();
      }
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

typedef pair<string,string> Label_t;

class Scenario : public Writable
{
  public:
    static vector<Label_t>  __labels;
    static vector<uint64_t> __counts;

  public:

    static void init(void) {
      __labels.push_back( Label_t("1.0","1 Run") );                          // 0
      __labels.push_back( Label_t("2.1","2 Runs (1 suit)") );                // 1
      __labels.push_back( Label_t("2.2","2 Runs (2 suits)") );               // 2
      __labels.push_back( Label_t("3.1","3 Runs (1 suit)") );                // 3
      __labels.push_back( Label_t("3.2","3 Runs (2 suits)") );               // 4
      __labels.push_back( Label_t("3.3","3 Runs (3 suits)") );               // 5
      __labels.push_back( Label_t("4.1","1 Run 1 Set (exclusive suits)") );  // 6
      __labels.push_back( Label_t("4.2","1 Run 1 Set (suits overlap)") );    // 7
      __labels.push_back( Label_t("???","Something else") );
      for(int i=0; i<__labels.size(); ++i) __counts.push_back(0);
    }

    static void summary(ostream &s)
    {
      uint64_t total = 0;
      s << endl;
      for(int i=0; i<__labels.size(); ++i)
      {
        s << __labels[i].first << " " << setw(20) << left << __labels[i].second << ": " << __counts[i] << endl;
        total += __counts[i];
      }
      s <<  endl << "Total: " << total << endl << endl;
    }

    Scenario(const Runs &runs, const Sets &sets)
    {
      _id = __labels.size() - 1;
      switch(runs.count())
      {
        case 0:
          switch(sets.count()) 
          {
            case 0: break;
            case 1: break;
            case 2: break;
            case 3: break;
          }
          break;
        case 1:
          switch(sets.count()) 
          {
            case 0: _id = 0; break;
            case 1:  
              if( sets[0].ncards() == 4 )
              {
                _id = ( runs[0].contains(sets[0].rank()) ? 6 : 7 );
              }
              else // set contains 3 cards
              {
                _id = ( runs[0].contains(sets[0].rank() && sets[0].contains(runs[0].suit())) ? 7 : 6 );
              }
              break;
            case 2: break;
            case 3: break;
          }
          break;
        case 2:
          switch(sets.count()) 
          {
            case 0: _id = runs.nsuits(); break;
            case 1: break;
            case 2: break;
            case 3: break;
          }
          break;
        case 3:
          if( runs.ncards() == 10 )
          {
            _id = 2+runs.nsuits();
          }
          else
          {
            switch(sets.count()) 
            {
              case 0: break;
              case 1: break;
              case 2: break;
              case 3: break;
            }
          }
          break;
      }

      if(trace) {
        cout << "Scenario: " << _id << endl;
        exit(1);
      }
      __counts[_id] += 1;
    }

    void write(ostream &s) const { s << __labels[_id].first; }

  private:
    int _id;
};

vector<Label_t>  Scenario::__labels;
vector<uint64_t> Scenario::__counts;


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

  Scenario::init();

  
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
    
    Hand hand(stoul(line,0,16));

    if(trace) cout << hand << endl;

    Runs runs(hand);
    Sets sets(hand);

    Scenario scenario(runs,sets);

    cout << hand << "  " << setw(5) << scenario << "   " << runs << " " << sets << endl;
  }

  Scenario::summary(cout);
}

