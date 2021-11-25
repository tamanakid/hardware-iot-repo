#ifndef _SCHEDULER_H
#define _SCHEDULER_H

/* Library Dependencies */

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

#ifdef __cplusplus
  extern "C" {
    schedulerTask *scheduler_add (int schedule_ticks, taskCallback callback, bool is_active);
    void scheduler_run ();
    void scheduler_activate (schedulerTask *task);
    void scheduler_deactivate (schedulerTask *task);

  }
#endif

#endif // _SCHEDULER_H
