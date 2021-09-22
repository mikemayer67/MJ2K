#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <strings.h>

// Macros/Constants

#define bool int
#define true 1
#define false 0

// Globals

#define MAX_ROCK 100
#define MAX_POSSIBLE_AGE (3*MAX_ROCK-3)

// Shared resources

pthread_mutex_t gRockLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gPrintLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gBestLock = PTHREAD_MUTEX_INITIALIZER;

#define LOCK_PRINT pthread_mutex_lock(&gPrintLock);
#define UNLOCK_PRINT pthread_mutex_unlock(&gPrintLock);

#define LOCK_ROCKS pthread_mutex_lock(&gRockLock);
#define UNLOCK_ROCKS pthread_mutex_unlock(&gRockLock);

#define LOCK_BEST pthread_mutex_lock(&gBestLock);
#define UNLOCK_BEST pthread_mutex_unlock(&gBestLock);

typedef int RockSet[7];

int     gBestSolution = 0;  // maximum solution age found so far
int     gNextMaxRock = 7;   // next max rock weight for thread to process

////////////////////////////////////////////////////////////////////////////////
// RockSet functions
////////////////////////////////////////////////////////////////////////////////

void print_rocks(RockSet rocks)
{
  LOCK_PRINT;
  for(int i=0; i<7; ++i) {
    printf(" %3d", rocks[i]);
  }
  UNLOCK_PRINT;
}

bool init_rocks(RockSet rocks)
{
  LOCK_ROCKS;
  int max_rock = gNextMaxRock++;
  LOCK_PRINT;
  if(rocks[6]>0) {
    printf("Starting max rock %d (finished %d)\n",max_rock,rocks[6]);
  }
  else
  {
    printf("Starting max rock = %d\n",max_rock);
  }

  UNLOCK_PRINT;
  UNLOCK_ROCKS;

  if(max_rock > MAX_ROCK) return false;

  for(int i=0; i<6; ++i) rocks[i] = i;
  rocks[6] = max_rock;

  return true;
}

bool advance_rock(int n, RockSet rocks)
{
  if( rocks[n] == rocks[n+1] - 1 )
  {
    // rock n is at maximum value based on subsequent rock
    if(n == 0) return false;  // no prior rock
    // try to advance the prior rock
    return advance_rock(n-1, rocks);
  }
  rocks[n] += 1;  // advance rock n
  // and all subsequent rocks except final
  for(int i=n+1; i<6; ++i) rocks[i] = rocks[i-1] + 1;

  return true;
}

bool advance_rocks(RockSet rocks)
{
  return advance_rock(5,rocks) || init_rocks(rocks);
}

////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////

void *play_with_rocks(void *args)
{
  unsigned char can_weigh[1+MAX_POSSIBLE_AGE];
  RockSet rocks;
  memset(rocks,0,sizeof(RockSet));

  for(bool ok=init_rocks(rocks); ok; ok=advance_rocks(rocks))
  {
    memset(can_weigh,0,1+MAX_POSSIBLE_AGE);

    for(int i=0; i<5; ++i) {
      int r1 = rocks[i];
      for(int j=1+i; j<6; ++j) {
        int r2 = rocks[j];
        for(int k=1+j; k<7; ++k) {
          int r3 = rocks[k];

          can_weigh[r1] = 1;
          can_weigh[r2] = 1;
          can_weigh[r3] = 1;

          can_weigh[r1+r2] = 1;
          can_weigh[r1+r3] = 1;
          can_weigh[r2+r3] = 1;

          can_weigh[r2-r1] = 1;
          can_weigh[r3-r1] = 1;
          can_weigh[r3-r2] = 1;

          can_weigh[r1+r2+r3] = 1;

          can_weigh[r3-r1+r2] = 1;
          can_weigh[r3-r2+r1] = 1;
          can_weigh[abs(r1+r2-r3)] = 1;
        }
      }
    }

    int max_age = rocks[4] + rocks[5] + rocks[6];

    for(int age=1; age<=max_age; ++age)
    {
      if(can_weigh[age] == 0) {
        max_age = age-1;
      }
    }

    LOCK_BEST;
    if(max_age > gBestSolution)
    {
      gBestSolution = max_age;

      LOCK_PRINT;
      printf("  New max age [%d] found: %d %d %d %d %d %d %d\n", max_age,
          rocks[0], rocks[1], rocks[2], rocks[3], rocks[4], rocks[5], rocks[6]);
      UNLOCK_PRINT;
    }
    UNLOCK_BEST;
  }

  return NULL;
}

int main(int argc,char **argv)
{
  pthread_t tid[4];
  for(int i=0; i<4; ++i)
  {
    int rc = pthread_create(&(tid[i]), NULL, play_with_rocks, NULL);
    if(rc != 0)
    {
      LOCK_PRINT;
      printf("Failed to start thread %d: %s\n", i, strerror(errno));
      UNLOCK_PRINT;
      exit(1);
    }
  }

  for(int i=0; i<4; ++i)
  {
    pthread_join(tid[i],NULL);
    LOCK_PRINT;
    printf("Thread %d complete\n", i);
    UNLOCK_PRINT;
  }

  return 0;
}


