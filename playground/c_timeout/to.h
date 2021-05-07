#ifndef _TO_H
#define _TO_H

typedef void (*timeout_func)(void *);

extern int timeout_run(timeout_func func, void *user_data, int timeout);

#endif // _TO_H
