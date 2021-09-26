#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stdbool.h>


/* Constants */
#define MAX_CONCURRENT_TASKS 10


/* Type definitions */

typedef void (*taskCallback)(void);

typedef struct {
  int schedule_ticks;
  taskCallback callback;
  bool is_active;
  int remaining_ticks;
} schedulerTask;


/* Function declarations */

schedulerTask *scheduler_add (int schedule_ticks, taskCallback callback, bool is_active);

void scheduler_run ();


#endif
