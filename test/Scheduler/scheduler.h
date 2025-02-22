#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

#define MAX_TASKS 10
#define CMD_BUFFER_SIZE 64
#define MAX_REGISTERED_TASKS 10

typedef void (*TaskFunction)();

typedef struct {
    TaskFunction function;
    char name[20];
    unsigned long interval;
    unsigned long lastRun;
    int priority;
    bool active;
} ScheduledTask;

typedef struct {
    char name[20];
    TaskFunction function;
} RegisteredTask;

// Global variables declaration (extern)
extern ScheduledTask taskList[MAX_TASKS];
extern RegisteredTask registeredTasks[MAX_REGISTERED_TASKS];
extern int taskCount;
extern int registeredTaskCount;
extern bool isPaused;
extern char commandBuffer[CMD_BUFFER_SIZE];

// Function prototypes (unchanged from original)
void scheduler_init();
void scheduler_register_task(const char* name, TaskFunction function);
void scheduler_add_task(const char* name, int priority, unsigned long interval);
void scheduler_remove_task(const char* name);
void scheduler_run();
void scheduler_inspect();
void scheduler_handle_command();

#endif