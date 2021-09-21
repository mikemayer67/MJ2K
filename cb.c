#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Macros/Constants

#define BOOL int
#define TRUE 1
#define FALSE 0

// Globals

#define MAX_ROCK 100
#define MAX_AGE (3*MAX_ROCK-3)

int gRockSet[7];  // weights of the current set of 7 rocks


////////////////////////////////////////////////////////////////////////////////
// RockSet functions
////////////////////////////////////////////////////////////////////////////////

void print_rocks(void)
{
  for(int i=0; i<7; ++i) {
    printf(" %3d", gRockSet[i]);
  }
}

void initialize_rocks(void)
{
  for(int i=0; i<7; ++i) gRockSet[i] = i+1;
}

int max_age(void)
{
  return gRockSet[4] + gRockSet[5] + gRockSet[6];
}

BOOL advance_rock(int n)
{
  if( gRockSet[n] == gRockSet[n+1] - 1 )
  {
    // rock n is at maximum value based on subsequent rock
    if( n > 0 ) return advance_rock(n-1);   // try to advance the prior rock
    return FALSE;                           // no prior rocks to advance
  }
  gRockSet[n] += 1;  // advance rock n
  // and all subsequent rocks except final
  for(int i=n+1; i<6; ++i) gRockSet[i] = gRockSet[i-1] + 1;

  return TRUE;
}

BOOL advance_max_rock(void)
{
  if( gRockSet[6] == MAX_ROCK ) return FALSE;

  gRockSet[6] += 1;
  for(int i=0; i<6; ++i) gRockSet[i] = i+1;
  return TRUE;
}

BOOL advance_rocks(void)
{
  return advance_rock(5) || advance_max_rock();
}


////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////

int main(int argc,char **argv)
{
  char weighable_ages[MAX_AGE];

  int max_weighable_age_found=0;

  initialize_rocks();
  do {
    int max_age = gRockSet[4] + gRockSet[5] + gRockSet[6];

    memset(weighable_ages,0,MAX_AGE);

    for(int i=0; i<5; ++i) {
      int r1 = gRockSet[i];
      for(int j=1+i; j<6; ++j) {
        int r2 = gRockSet[j];
        for(int k=1+j; k<7; ++k) {
          int r3 = gRockSet[k];

          int ages[] = {
            r1, r2, r3,
            r1+r2, r1+r3, r2+r3,
            r2-r1, r3-r2, r3-r1,
            r1+r2+r3,
            abs(r1+r2-r3), abs(r1+r3-r2), abs(r2+r3-r1)
          };

          for(int t=0; t<13; ++t) {
            if( ages[t] > 0) weighable_ages[ages[t]-1] = 1;
          }
        }
      }
    }

    for(int age=1; age<=max_age; ++age)
    {
      if(weighable_ages[age-1] == 0) {
        if( age-1 > max_weighable_age_found )
        {
          max_weighable_age_found = age-1;
          printf("New max age [%d] found: %d %d %d %d %d %d %d\n", age-1,
              gRockSet[0], 
              gRockSet[1],
              gRockSet[2],
              gRockSet[3],
              gRockSet[4],
              gRockSet[5],
              gRockSet[6]);
        }
        break;
      }
    }
  }
  while( advance_rocks() );

  return 0;
}
