#include <stddef.h>
#include "scheduler.h"


schedulerTask tasks[MAX_CONCURRENT_TASKS];


/**
 * @brief adds a new task to the tasks list
 * @param schedule_ticks - ticks/loops that takes for the task to execute
 * @param taskCallback - reference to the task function to be executed
 * @param is_active - initial value of task status
 * @return reference to schedulerTask instance created (Which can be modified)
 */
schedulerTask *scheduler_add (int schedule_ticks, taskCallback callback, bool is_active) {
  
  for (int i = 0; i < MAX_CONCURRENT_TASKS; i++) {
    if (tasks[i].callback == NULL) {
      
      schedulerTask new_task = { schedule_ticks, callback, is_active, schedule_ticks };
      tasks[i] = new_task;

      return &tasks[i];
    }
  }
  
  return NULL;
}


/**
 * @brief activates a task to be run by scheduler (could be done directly in pointer)
 * @param task - task to activate
 */
void scheduler_activate(schedulerTask *task) {
  if (!task->is_active) {
    task->is_active = true;
  }
}


/**
 * @brief deactivates a task to avoid being run by scheduler (could be done directly in pointer)
 * @param task - task to deactivate
 */
void scheduler_deactivate(schedulerTask *task) {
  if (task->is_active) {
    task->is_active = false;
  }
}



/**
 * @brief iterates over tasks array and executes active callbacks
 */
void scheduler_run () {
  
  for (int i = 0; i < MAX_CONCURRENT_TASKS; i++) {
    schedulerTask *task = &tasks[i];
    
    if (task->callback != NULL) {
      // Taks exists in array
      if (task->is_active == true) {
        task->remaining_ticks = task->remaining_ticks - 1;
  
        if (task->remaining_ticks == 0) {
          // Call task and restart remaining_ticks counter
          task->callback();
          task->remaining_ticks = task->schedule_ticks;
        }
      }
    } else {
      // Task with null callback: No more callbacks pending. Exiting function.
      return;
    }
  }
}
