#include <iostream>
#include <iomanip>
#include <bitset>

using namespace std;

uint64_t nhands = 0;
uint64_t ngin   = 0;
uint64_t ngin_sets_only = 0;
uint64_t ngin_runs_only = 0;
uint64_t ngin_one_one   = 0;

uint64_t nugly = 0;

struct Run
{
  int suit;
  int rank;   // end of run
  int length;

  bool contains(int r) {
    return ( ( r > (rank-length) ) && ( r <= rank ) );
};

struct Set
{
  int rank;
  int length;
  int suits;  // bitfield
};

class show_hand
{
  public:
    show_hand(uint64_t _hand) : hand(_hand) {}
    void dump(ostream &s) const
    {
      uint64_t mask = 0x1fff;
      s << bitset<13>((hand & (mask << 39)) >> 39) << " "
        << bitset<13>((hand & (mask << 26)) >> 26) << " "
        << bitset<13>((hand & (mask << 13)) >> 13) << " "
        << bitset<13>(hand & 0x1fff);
    }
  private:
    uint64_t hand;
};
ostream &operator<<(ostream &s, const show_hand &x) { x.dump(s); return s; }

class show_runs
{
  public:
    show_runs(int nruns, Run *runs) 
    {
      memset(this,0,sizeof(show_runs));

      int t;

      for(int i=0; i<nruns; ++i)
      {
        int suit = runs[i].suit; 
        int j = run_count[suit];
        int length = runs[i].length;

        run_count[suit]  += 1;
        card_count[suit] += length;
        lengths[suit][j]  = length;
      }

      for(int i=0; i<4; ++i) {
        for(int j=0; j<run_count[i]-1; ++j) {
          for(int k=j+1; k<run_count[i]; ++k) {
            if( lengths[i][k] > lengths[i][j] ) {
              t=lengths[i][k]; lengths[i][k]=lengths[i][j]; lengths[i][j]=t;
            }
          }
        }
      }

      for(int i=0; i<3; ++i) {
        for(int j=i+1; j<4; ++j) {
          if( card_count[j] > card_count[i] )
          {
            t=run_count[i];  run_count[i] =run_count[j];  run_count[j] =t;
            t=card_count[i]; card_count[i]=card_count[j]; card_count[j]=t;
            for(int k=0; k<3; ++k) {
              t=lengths[i][k]; lengths[i][k]=lengths[j][k]; lengths[j][k]=t;
            }
          }
        }
      }
    }
    void dump(ostream &s) const
    {
      for(int i=0; i<4; ++i) {
        for(int j=0; j<run_count[i]; ++j) {
          if( j==0 && i>0 ) s << "| ";
          s << "R" << lengths[i][j] << " ";
        }
      }
    }
  private:
    int run_count[4];
    int card_count[4];
    int lengths[4][3];
};
ostream &operator<<(ostream &s, const show_runs &x) { x.dump(s); return s; }


void evaluate(uint64_t hand)
{
  nhands += 1;

  if(nhands%100000000 == 0) 
    cout << show_hand(hand) << "  " << ngin << "(" << ngin_sets_only << "/" << ngin_runs_only << ")  " << nugly << endl;
  
  // check sets first.
  //   if we can make three sets, runs are irrelevant.

  int suits[13];
  memset(suits,0,13*sizeof(int));

  uint64_t mask = 0x1;
  for(int suit=0; suit<4; ++suit)
  {
    for(int rank=0; rank<13; ++rank, mask <<= 1)
    {
      if(hand&mask) {
        suits[rank] |= (0x01 << suit);
      }
    }
  }

  int nsets = 0;
  Set sets[3];

  int cards_in_sets = 0;
  for(int rank=0; rank<13; ++rank)
  {
    switch( suits[rank] )
    {
      case 0x0f:
        cards_in_sets += 4;
        sets[nsets].length = 4;
        sets[nsets].rank = rank;
        sets[nsets].suits = suits[rank];
        nsets += 1;
        break;
      case 0x0e: case 0x0d: case 0x0b: case 0x07:
        cards_in_sets += 3;
        sets[nsets].length = 3;
        sets[nsets].rank = rank;
        sets[nsets].suits = suits[rank];
        nsets += 1;
        break;
      default:
        break;
    }
  }

  if(cards_in_sets == 10)
  {
    // all cards are in a set
    ngin_sets_only += 1;
    ngin += 1;
    return;
  }

  //  look for runs.

  int cards_in_runs = 0;
  int cards_in_both = 0;

  int nruns = 0;
  Run runs[3];

  int  run_length  = 0;
  int  dups_in_run = 0;
  bool in_run      = false;

  mask = 0x01;
  for(int suit=0; suit<4; ++suit)
  {
    for(int rank=0; rank<13; ++rank, mask <<= 1)
    {
      if(hand&mask) 
      {
        in_run = true;
        run_length += 1;

        if(nrank[rank]) dups_in_run += 1;
      }
      else
      {
        in_run = false;
      }

      if( !in_run || rank==12 )
      { 
        if( run_length > 2 )
        {
          assert(nruns<3); // cannot create 4 runs with only 10 cards

          runs[nruns].suit   = suit;
          runs[nruns].rank   = (in_run ? rank : rank-1);
          runs[nruns].length = run_length;
          nruns += 1;
          cards_in_runs += run_length;
          cards_in_both += dups_in_run;
        }
        in_run      = false;
        run_length  = 0;
        dups_in_run = 0;
      }
    }
  }
  
  if(cards_in_runs == 10)
  {
    // all cards are in a run
    ngin_runs_only += 1;
    ngin += 1;

    return;
  }

  if( cards_in_runs + cards_in_sets - cards_in_both < 10 )
  {
    // at least one card is not in a run or a set... no gin
    return;
  }

  // start examining cases:

  // 4. 1 Run/1 Set
  if( nruns == 1 && nsets == 1 )
  {
    if( cards_in_runs + cards_in_sets - cards_in_both == 10 )
    {
      if( cards_in_both == 0
          || sets[0].length == 4
          || sets[0].rank == runs[0].rank
          || sets[0].rank == runs[0].rank - (runs[0].length-1) )
      {
        ngin_one_one += 1;
        ngin += 1;

        cout << ngin_one_one << ": " << show_hand(hand) 
          << " : R" << runs[0].length << " S" << sets[0].length
          << " : " << sets[0].rank << " (" << runs[0].rank - runs[0].length + 1 << "-" << runs[0].rank << ")"
          << endl;
      }
    }
  }

  // 5.1 2 Runs/1 Set, both 
  
  if( nruns==2 && nsets==1 && sets[0].length==3)
  {
    if( runs[0].contains(sets[0].rank) || runs[1].contains(sets[0].rank) ) {

    }
  }

  // no cards in both sets and runs

  if(cards_in_both == 0)
  {
    if(cards_in_runs + cards_in_sets == 10)
    {
      // all cards are in a set or a run and no cards are in both... we're good
      ngin += 1;
      return;
    }
    else
    {
      // at least one card is not in a run or a set... no gin
      return;
    }
  }
  
  // ... starts getting tricky, there is at least one card that is in both a run and a set


  // ... and now the real fun, all cards are in a run or a set and at least one is in both

  // for now, just count it as ugly... TODO:: we need to resolve all of these
  nugly += 1;
  return;
}

void deal(int firstcard, int ncards, uint64_t hand)
{
  uint64_t mask = 1;

  if(ncards == 10) { 
    evaluate(hand);
  }
  else
  {
    for(int card=firstcard; card<=ncards+42; ++card)
    {
      deal(card+1, ncards+1, hand|(mask<<card));
    }
  }
}

int main(int argc,char **argv)
{
  deal(0,0,0);

  cout << ngin << " / " << nhands << endl
    << endl
    << "       sets only: " << ngin_sets_only << endl
    << "       runs only: " << ngin_runs_only << endl
    << "   1 run / 1 set: " << ngin_one_one << endl
    << "to be worked out: " << nugly << endl
    << endl;

  return 0;
}
