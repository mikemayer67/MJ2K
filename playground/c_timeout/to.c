#include "to.h"

#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

typedef struct 
{
  int              done;
  pthread_mutex_t *mutex;
  pthread_cond_t  *cond;

  timeout_func     func;
  void            *data;
} to_t;


void *timeout_worker(void *to_data)
{
  to_t *to = (to_t *)to_data;

  int oldtype;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

  printf("worker: launching function\n");
  to->func(to->data);
  printf("worker: function complete\n");


  printf("worker:: attemping to obtain lock\n");
  pthread_mutex_lock( to->mutex );
  printf("worker:: lock obtained (done=%d)\n", to->done);

  to->done = 1;

  printf("worker:: releasing lock (done=%d)\n", to->done);
  pthread_mutex_unlock( to->mutex );
  printf("worker:: lock released\n");

  pthread_cond_signal(to->cond);

  return NULL;
}

int timeout_run(timeout_func func, void *user_data, int timeout)
{
  pthread_t       tid;
  int             err;

  struct timespec end_time;
  clock_gettime(CLOCK_REALTIME, &end_time);
  end_time.tv_sec += timeout;

  pthread_mutex_t to_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t  to_cond  = PTHREAD_COND_INITIALIZER;

  to_t to;
  to.func  = func;
  to.data  = user_data;
  to.done  = 0;
  to.mutex = &to_mutex;
  to.cond  = &to_cond;

  printf("main:: attemping to obtain lock\n");
  pthread_mutex_lock( to.mutex );
  printf("main:: lock obtained (done=%d)\n", to.done);

  pthread_create( &tid, NULL, timeout_worker, &to );

  printf("main:: worker thread created\n");

  while( ! to.done )
  {
    printf("main:: looping until done(%d)\n",to.done);

    err = pthread_cond_timedwait( &to_cond, &to_mutex, &end_time );

    if(err == ETIMEDOUT)
    {
      printf("main:: timed out\n");
      pthread_cancel(tid);
      break;
    }
    else if(err)
    {
      printf("main:: cond signaled (err=%d): %s\n", err,strerror(err));
    }
    else
    {
      printf("main:: cond signaled: (done=%d)\n", to.done);
    }
  }

  printf("main:: releasing lock\n");
  pthread_mutex_unlock( &(to_mutex) );
  printf("main:: lock released\n");

  return err != ETIMEDOUT;
}
