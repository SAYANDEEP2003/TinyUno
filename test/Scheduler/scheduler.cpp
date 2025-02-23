#include "scheduler.h"
#include <string.h>

ScheduledTask taskList[MAX_TASKS];
RegisteredTask registeredTasks[MAX_REGISTERED_TASKS];
int taskCount = 0;
int registeredTaskCount = 0;
bool isPaused = false;
char commandBuffer[CMD_BUFFER_SIZE];

void scheduler_init() {
    taskCount = 0;
    registeredTaskCount = 0;
    isPaused = false;
    memset(commandBuffer, 0, CMD_BUFFER_SIZE);
}

void scheduler_register_task(const char* name, TaskFunction function) {
    if (registeredTaskCount < MAX_REGISTERED_TASKS) {
        strncpy(registeredTasks[registeredTaskCount].name, name, 19);
        registeredTasks[registeredTaskCount].function = function;
        registeredTaskCount++;
    }
}

void scheduler_add_task(const char* name, unsigned long duration, int priority) {
    if (taskCount >= MAX_TASKS) return;

    // Find the registered task by name
    TaskFunction taskFunction = nullptr;
    for (int i = 0; i < registeredTaskCount; i++) {
        if (strcmp(registeredTasks[i].name, name) == 0) {
            taskFunction = registeredTasks[i].function;
            break;
        }
    }

    if (taskFunction != nullptr) {
        strncpy(taskList[taskCount].name, name, 19);
        taskList[taskCount].function = taskFunction;
        taskList[taskCount].duration = duration;
        taskList[taskCount].startTime = 0;
        taskList[taskCount].endTime = 0;
        taskList[taskCount].priority = priority;
        taskList[taskCount].active = true;
        taskCount++;
    }
}

void scheduler_remove_task(const char* name) {
    for (int i = 0; i < taskCount; i++) {
        if (strcmp(taskList[i].name, name) == 0) {
            for (int j = i; j < taskCount - 1; j++) {
                taskList[j] = taskList[j + 1];
            }
            taskCount--;
            Serial.print("Removed task: ");
            Serial.println(name);
            return;
        }
    }
    Serial.println("Task not found");
}

void scheduler_run() {
    if (isPaused || taskCount == 0) return;
    unsigned long currentMillis = millis();

    // Find the highest priority active task
    int highestPriority = -1;
    int highestPriorityIndex = -1;
    for (int i = 0; i < taskCount; i++) {
        if (taskList[i].active && (highestPriority == -1 || taskList[i].priority < highestPriority)) {
            highestPriority = taskList[i].priority;
            highestPriorityIndex = i;
        }
    }

    // Execute the highest priority active task
    if (highestPriorityIndex != -1) {
        ScheduledTask* task = &taskList[highestPriorityIndex];
        if (task->startTime == 0) {
            task->startTime = currentMillis;
            task->endTime = task->startTime + task->duration;
        }

        // Run the task function
        task->function();

        // Check if duration has elapsed
        if (currentMillis >= task->endTime) {
            task->active = false; // Deactivate the task
            task->startTime = 0;
            task->endTime = 0;
        }
    }
}

void scheduler_inspect() {
    isPaused = true;
    Serial.println("\n--- Task List ---");
    for (int i = 0; i < taskCount; i++) {
        Serial.print("Name: ");
        Serial.print(taskList[i].name);
        Serial.print(" | Duration: ");
        Serial.print(taskList[i].duration);
        Serial.print("ms | Priority: ");
        Serial.println(taskList[i].priority);
    }
    Serial.println("-----------------");
}

void scheduler_handle_command() {
    static int bufferIndex = 0;

    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n' || bufferIndex >= CMD_BUFFER_SIZE - 1) {
            commandBuffer[bufferIndex] = '\0';
            bufferIndex = 0;

            char cmd[10];
            char taskName[20];
            unsigned long duration = DEFAULT_DURATION;
            int priority = -1; // Default to -1 to indicate no priority given

            sscanf(commandBuffer, "%s", cmd);

            if (strcmp(cmd, "exec") == 0) {
                char* ptr = commandBuffer + 4;
                sscanf(ptr, "%s", taskName);

                // Parse parameters like "-t 5000 -p 2"
                char* param = strtok(ptr, " ");
                while ((param = strtok(NULL, " ")) != NULL) {
                    if (strcmp(param, "-t") == 0) {
                        duration = atol(strtok(NULL, " "));
                    } else if (strcmp(param, "-p") == 0) {
                        priority = atoi(strtok(NULL, " "));
                    }
                }

                // If no priority is given, set it to one less than the lowest priority
                if (priority == -1) {
                    priority = MAX_TASKS;
                    for (int i = 0; i < taskCount; i++) {
                        if (taskList[i].priority < priority) {
                            priority = taskList[i].priority;
                        }
                    }
                    priority--;
                }

                scheduler_add_task(taskName, duration, priority);
                Serial.print("Added task: ");
                Serial.println(taskName);
            } else if (strcmp(cmd, "halt") == 0) {
                sscanf(commandBuffer + 5, "%s", taskName);
                scheduler_remove_task(taskName);
            } else if (strcmp(cmd, "inspect") == 0) {
                scheduler_inspect();
            } else if (strcmp(cmd, "cont") == 0) {
                isPaused = false;
                Serial.println("Resuming execution...");
            }

            memset(commandBuffer, 0, CMD_BUFFER_SIZE);
        } else {
            commandBuffer[bufferIndex++] = c;
        }
    }
}
