#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
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

    bool can_drop(int rank) const
    {
      return ( ncards() > 3 ) && ends_with(rank) ;
    }

    bool ends_with(int rank) const
    {
      return ( rank == _start || rank == _end - 1 );
    }

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

    bool contains(int rank) const
    {
      for(int i=0; i<_nruns; ++i) {
        if( _runs[i].contains(rank) ) return true;
      }
      return false;
    }

    bool can_drop(int rank) const
    {
      for(int i=0; i<_nruns; ++i) {
        if( _runs[i].can_drop(rank) ) return true;
      }
      return false;
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



string evaluate_scenario(const Runs &runs, const Sets &sets)
{
  int nrun = runs.count();
  int nset = sets.count();
  int overlap = runs.ncards() + sets.ncards() - 10;

  if( nrun == 1 && nset == 0 && runs.ncards() == 10 ) {
    return "1.1";
  }
  else if (nrun == 2 && nset == 0 && runs.ncards() == 10 ) {
    switch(runs.nsuits())
    {
      case 1: return "2.1"; break;
      case 2: return "2.2"; break;
    }
  }
  else if (nrun == 3 and runs.ncards() == 10)
  {
    switch(runs.nsuits())
    {
      case 1: return "3.1"; break;
      case 2: return "3.2"; break;
      case 3: return "3.3"; break;
    }
  }
  else if (nset == 3 and sets.ncards() == 10)
  {
    return "4.1";
  }
  else if (nrun == 1 && nset == 1) // 5
  {
    if( sets[0].contains(runs[0].suit()) ) // 5.2
    {
      return ( overlap == 0 ? "5.2.1" : "5.2.2" );
    }
    else // 5.1
    {
      if(overlap == 0) { return "5.1.1"; }
    }
  }
  else if( nrun == 2 && nset == 1) // 6
  {
    if( runs[0].suit() == runs[1].suit() ) // 6.1 and 6.2
    {
      if( sets[0].contains(runs[0].suit()) ) { // 6.1
        if( overlap == 0 ) { // 6.1.1
          if( sets[0].ncards() == 4 ) { return "6.1.1.1"; }
          else                       { return "6.1.1.2"; }
        } else { // 6.1.2
          if( sets[0].ncards() == 4 ) { return "6.1.2.1"; }
          else if (runs[0].ncards() == 4 && runs[1].ncards() == 4 ) { return "6.1.2.2"; }
          else { return "6.1.2.3"; }
        }
      }
      else // 6.2
      {
        return "6.2";
      }
    }
    else // 6.3 and 6.4
    {
      if( sets[0].contains(runs[0].suit()) && sets[0].contains(runs[1].suit()) ) // 6.3
      {
        if(overlap == 0) { 
          if( sets[0].ncards() == 4 ) { return "6.3.1.1"; }
          else if ( sets[0].ncards() == 3 ) { return "6.3.1.2"; }
          cout << "6.3.1.? " << runs.details() << " " << sets.details() << endl;
          exit(1);

        }
        if(overlap == 1) { return "6.3.2"; }
        if(overlap == 2) { 
          if(sets[0].ncards() == 4)
          {
            if(runs[0].ncards() == 4 && runs[1].ncards() == 4 ) { return "6.3.3.1"; }
            if(runs[0].ncards() == 5 && runs[1].ncards() == 3 ) { return "6.3.3.2"; }
            if(runs[0].ncards() == 3 && runs[1].ncards() == 5 ) { return "6.3.3.2"; }
            cout << "6.3.3.? " << runs.details() << " " << sets.details() << endl;
            exit(1);
          }
          else
          {
            if(runs[0].ncards() == 5 && runs[1].ncards() == 4 ) { return "6.3.3.3"; }
            if(runs[0].ncards() == 4 && runs[1].ncards() == 5 ) { return "6.3.3.3"; }
            cout << "6.3.3.? " << runs.details() << " " << sets.details() << endl;
            exit(1);
          }
        }
      }
      else // 6.4
      {
        return "6.4";
      }
    }
  }
  else if( nrun == 1 && nset == 2 ) // 7
  {
    int run_suit = runs[0].suit();
    int run_suit_in_set[2] = { sets[0].contains(run_suit), sets[1].contains(run_suit) };
    if( run_suit_in_set[0] && run_suit_in_set[1] ) // 7.3
    {
      if( overlap == 0 ) // 7.3.1
      {
        return "7.3.1";
      }
      else if(overlap == 1) // 7.3.2
      {
        return "7.3.2";
      }
      else if(overlap == 2) // 7.3.3
      {
        if( runs[0].ncards() == 4 ) { return "7.3.3.1"; }
        if( runs[0].ncards() == 5 ) { return "7.3.3.2"; }
        if( runs[0].ncards() == 6 ) { return "7.3.3.3"; }
        cout << "7.3.3.? " << runs.details() << " " << sets.details() << endl;
        exit(1);
      }
      cout << "7.3.? " << runs.details() << " " << sets.details() << endl;
      exit(1);
    }
    else if( run_suit_in_set[0] || run_suit_in_set[1] ) // 7.2
    {
      if(overlap == 0) // 7.2.1
      {
        if     ( runs[0].ncards() == 3 ) { return "7.2.1.1"; }
        else if( runs[0].ncards() == 4 ) { return "7.2.1.2"; }
        cout << "7.2.1.? " << runs.details() << " " << sets.details() << endl;
        exit(1);
      }
      else if(overlap == 1) // 7.2.2
      {
        return "7.2.2";
      }
      cout << "7.2.? " << runs.details() << " " << sets.details() << endl;
      exit(1);
    }
    else // 7.1
    {
      return "7.1";
    }
  }

  return "???";
}


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

  // Scenarios

  map<string,string> scenarios;
  scenarios["1"]     = "One Run / No Sets";
  scenarios["2"]     = "Two Runs No Sets";
  scenarios["2.1"]   = "all same suit";
  scenarios["2.2"]   = "two suits";
  scenarios["3"]     = "Three Runs";
  scenarios["3.1"]   = "all same suit";
  scenarios["3.2"]   = "two suits";
  scenarios["3.3"]   = "three suits";
  scenarios["4"]     = "Three Sets";
  scenarios["5"]     = "One Run / One Set";
  scenarios["5.1"]   = "set does not include run suit";
  scenarios["5.1.1"] = "no overlap";
  scenarios["5.2"]   = "Set does include run suit";
  scenarios["5.2.1"] = "no overlap";
  scenarios["5.2.2"] = "overlap";
  scenarios["6"]     = "Two Runs / One Set";
  scenarios["6.1"]   = "runs are same suit / suit in set";
  scenarios["6.1.1"] = "no oerlap";
  scenarios["6.1.1.1"] = "R3/R3/S4";
  scenarios["6.1.1.2"] = "R4/R3/S3";
  scenarios["6.1.2"] = "overlap";
  scenarios["6.1.2.1"] = "R4/R3/S4";
  scenarios["6.1.2.2"] = "R4/$4/S3";
  scenarios["6.1.2.3"] = "R5/S3/S3";
  scenarios["6.2"]   = "Runs are same suit / suit not in set";
  scenarios["6.3"]   = "Runs are different suit / both in set";
  scenarios["6.3.1"] = "no overlap";
  scenarios["6.3.1.1"] = "R3/R3/S4";
  scenarios["6.3.1.2"] = "R4/R3/S3";
  scenarios["6.3.2"] = "one overlap";
  scenarios["6.3.3"] = "two overlap";
  scenarios["6.3.3.1"] = "R4/R4/S4";
  scenarios["6.3.3.2"] = "R5/R3/S4";
  scenarios["6.3.3.3"] = "R5/R4/S3";
  scenarios["6.4"]   = "Runs are different suit / one in set";
  scenarios["7"]     = "1 Run / 2 Sets";
  scenarios["7.1"]   = "run suit in neither set";
  scenarios["7.2"]   = "run suit in one set";
  scenarios["7.2.1"] = "no overlap`";
  scenarios["7.2.1.1"] = "R3/S4/S3";
  scenarios["7.2.1.2"] = "R4/S3/S3";
  scenarios["7.2.2"] = "one overlap";
  scenarios["7.3"]   = "run suit in both sets";
  scenarios["7.3.1"]   = "no overlap";
  scenarios["7.3.2"]   = "one overlap";
  scenarios["7.3.3"]   = "two overlap";
  scenarios["7.3.3.1"] = "R4/S4/S4";
  scenarios["7.3.3.2"] = "R5/S4/S3";
  scenarios["7.3.3.3"] = "R6/S3/S3";
  scenarios["???"]   = "missed case";


  map<string,uint32_t> counts;
  vector<string> keys;
  int description_length = 0;
  int key_length = 0;
  for(map<string,string>::const_iterator x=scenarios.begin(); x!=scenarios.end(); ++x)
  {
    string scenario = x->first;
    int len = x->second.length();
    if(len > description_length) description_length = len;
    len = x->first.length();
    if(len > key_length) key_length = len;

    counts[scenario] = 0;
    keys.push_back(scenario);

  }
  sort(keys.begin(), keys.end());
  counts["total"] = 0;


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

    string scenario = evaluate_scenario(runs,sets);

    cout << hand << "  " << setw(5) << scenario << "   " << runs << " " << sets << endl;

    counts["total"] += 1;
    while(scenario.length() > 0)
    {
      counts[scenario] += 1;
      size_t pos = scenario.find_last_of('.');
      if( pos == string::npos) pos = 0;
      scenario.erase(pos);
    }
  }

  cout << endl;

  for(vector<string>::const_iterator key=keys.begin(); key!=keys.end(); ++key)
  {
    cout << left << setw(key_length) << *key << " " 
      << left << setw(description_length) << scenarios[*key] << ": " 
      << setw(6) << counts[*key] << endl;
  }
  cout << endl << "Total: " << counts["total"] << endl << endl;
}
