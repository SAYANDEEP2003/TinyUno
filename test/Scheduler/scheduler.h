#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

#define MAX_TASKS 10
#define MAX_REGISTERED_TASKS 10
#define CMD_BUFFER_SIZE 100
#define DEFAULT_DURATION 3000

typedef void (*TaskFunction)();

struct ScheduledTask {
    char name[20];           // Task name
    TaskFunction function;   // Task function
    unsigned long duration;  // Time to run per cycle (ms)
    unsigned long startTime; // Time when the task started running
    unsigned long endTime;   // Time when the task should stop running
    int priority;            // Priority (higher = first)
    bool active;             // Whether the task is active
};

struct RegisteredTask {
    char name[20];           // Registered task name
    TaskFunction function;   // Registered function
};

extern ScheduledTask taskList[MAX_TASKS];
extern RegisteredTask registeredTasks[MAX_REGISTERED_TASKS];
extern int taskCount;
extern int registeredTaskCount;
extern bool isPaused;
extern char commandBuffer[CMD_BUFFER_SIZE];

void scheduler_init();
void scheduler_register_task(const char* name, TaskFunction function);
void scheduler_add_task(const char* name, unsigned long duration, int priority);
void scheduler_remove_task(const char* name);
void scheduler_run();
void scheduler_inspect();
void scheduler_handle_command();

#endif
