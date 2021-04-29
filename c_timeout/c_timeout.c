#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>

typedef struct
{
  int             done;
  pthread_mutex_t *mutex;
  pthread_cond_t  *cond;
} timeout_info_t ;

void *doit(void *userdata)
{
  timeout_info_t *to = (timeout_info_t *)userdata;

  int oldtype;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

  for(int i=0; i<10; ++i)
  {
    printf("doit:: %d\n",i);
    sleep(1);
  }
  printf("PSYCH...");

  printf("doit:: attemping to obtain lock\n");
  pthread_mutex_lock( to->mutex );
  printf("doit:: lock obtained (done=%d)\n", to->done);
  to->done = 1;
  printf("doit:: releasing lock (done=%d)\n", to->done);
  pthread_mutex_unlock( to->mutex );
  printf("doit:: lock released\n");
  pthread_cond_signal(to->cond);
  for(int i=0; i<40; ++i)
  {
    printf("doit:: %d again\n",i);
    sleep(1);
  }

  printf("doit:: attemping to obtain lock\n");
  pthread_mutex_lock( to->mutex );
  printf("doit:: lock obtained (done=%d)\n", to->done);
  to->done = 2;
  printf("doit:: releasing lock (done=%d)\n", to->done);
  pthread_mutex_unlock( to->mutex );
  printf("doit:: lock released\n");

  printf("doit:: sending cond signal\n");
  pthread_cond_signal(to->cond);
  printf("doit::signal sent\n");

  return NULL;
}

int main(int argc,char **argv)
{
  pthread_t tid;
  struct timespec to_time;
  int err;

  clock_gettime(CLOCK_REALTIME, &to_time);
  to_time.tv_sec += 30;

  pthread_mutex_t to_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t  to_cond  = PTHREAD_COND_INITIALIZER;

  timeout_info_t to;
  to.done  = 0;
  to.mutex = &to_mutex;
  to.cond  = &to_cond;


  printf("Starting\n");

  printf("main:: attemping to obtain lock\n");
  pthread_mutex_lock( &to_mutex );
  printf("main:: lock obtained (done=%d)\n", to.done);

  pthread_create( &tid, NULL, doit, &to );

  printf("main:: doit thread created\n");

  while( to.done < 2 )
  {
    printf("main:: looping until done(%d)\n",to.done);

    err = pthread_cond_timedwait( &to_cond, &to_mutex, &to_time );

    printf("main:: cond signaled (err=%d): %s\n", err,strerror(err));

    if(err == ETIMEDOUT)
    {
      printf("main:: timed out\n");
      pthread_cancel(tid);
      break;
    }

    if( !err) { 
      printf("main:: doit completed: (done=%d)\n", to.done);
    }
  }

  printf("main:: releasing lock\n");
  pthread_mutex_unlock( &(to_mutex) );
  printf("main:: lock released\n");

  printf("Done\n");

  return 0;
}
