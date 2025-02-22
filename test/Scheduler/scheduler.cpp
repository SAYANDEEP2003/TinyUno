#include "scheduler.h"
#include <Arduino.h>
#include <string.h>

// Global variables (identical to your original .ino)
ScheduledTask taskList[MAX_TASKS];
RegisteredTask registeredTasks[MAX_REGISTERED_TASKS];
int taskCount = 0;
int registeredTaskCount = 0;
bool isPaused = false;
char commandBuffer[CMD_BUFFER_SIZE];

// Original functions (copied verbatim)
void scheduler_init() {
    taskCount = 0;
    isPaused = false;
}

void scheduler_register_task(const char* name, TaskFunction function) {
    if (registeredTaskCount >= MAX_REGISTERED_TASKS) return;
    strncpy(registeredTasks[registeredTaskCount].name, name, 19);
    registeredTasks[registeredTaskCount].function = function;
    registeredTaskCount++;
}

void scheduler_add_task(const char* name, int priority, unsigned long interval) {
    if (taskCount >= MAX_TASKS) return;

    TaskFunction func = nullptr;
    for (int i = 0; i < registeredTaskCount; i++) {
        if (strcmp(registeredTasks[i].name, name) == 0) {
            func = registeredTasks[i].function;
            break;
        }
    }
    if (!func) {
        Serial.println("Error: Task not registered");
        return;
    }

    if (priority == -1) {
        int minPriority = 0;
        for (int i = 0; i < taskCount; i++) {
            if (taskList[i].priority < minPriority) minPriority = taskList[i].priority;
        }
        priority = minPriority - 1;
    }

    int i = taskCount - 1;
    while (i >= 0 && taskList[i].priority < priority) {
        taskList[i + 1] = taskList[i];
        i--;
    }

    strncpy(taskList[i+1].name, name, 19);
    taskList[i+1].function = func;
    taskList[i+1].interval = interval;
    taskList[i+1].priority = priority;
    taskList[i+1].lastRun = millis();
    taskList[i+1].active = true;
    taskCount++;

    Serial.print("Added task: ");
    Serial.println(name);
}

void scheduler_remove_task(const char* name) {
    for (int i = 0; i < taskCount; i++) {
        if (strcmp(taskList[i].name, name) == 0) {
            for (int j = i; j < taskCount-1; j++) {
                taskList[j] = taskList[j+1];
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
    if (isPaused) return;
    unsigned long currentMillis = millis();
    
    for (int i = 0; i < taskCount; i++) {
        if (taskList[i].active && (currentMillis - taskList[i].lastRun >= taskList[i].interval)) {
            taskList[i].function();
            taskList[i].lastRun = currentMillis;
        }
    }
}

void scheduler_inspect() {
    isPaused = true;
    Serial.println("\n--- Task List ---");
    for (int i = 0; i < taskCount; i++) {
        Serial.print("Name: ");
        Serial.print(taskList[i].name);
        Serial.print(" | Priority: ");
        Serial.print(taskList[i].priority);
        Serial.print(" | Interval: ");
        Serial.print(taskList[i].interval);
        Serial.print("ms | Active: ");
        Serial.println(taskList[i].active ? "Yes" : "No");
    }
    Serial.println("-----------------");
}

void scheduler_handle_command() {
    static int bufferIndex = 0;

    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n' || bufferIndex >= CMD_BUFFER_SIZE-1) {
            commandBuffer[bufferIndex] = '\0';
            bufferIndex = 0;

            char cmd[10];
            char taskName[20];
            int priority = -1;
            unsigned long interval = 0;

            sscanf(commandBuffer, "%s", cmd);

            if (strcmp(cmd, "exec") == 0) {
                char* ptr = commandBuffer + 4;
                sscanf(ptr, "%s", taskName);
                
                char* param = strtok(ptr, " ");
                while ((param = strtok(NULL, " ")) != NULL) {
                    if (strcmp(param, "-p") == 0) {
                        priority = atoi(strtok(NULL, " "));
                    }
                    if (strcmp(param, "-t") == 0) {
                        interval = atol(strtok(NULL, " "));
                    }
                }
                
                scheduler_add_task(taskName, priority, interval);
            }
            else if (strcmp(cmd, "halt") == 0) {
                sscanf(commandBuffer + 5, "%s", taskName);
                scheduler_remove_task(taskName);
            }
            else if (strcmp(cmd, "inspect") == 0) {
                scheduler_inspect();
            }
            else if (strcmp(cmd, "cont") == 0) {
                isPaused = false;
                Serial.println("Resuming execution...");
            }

            memset(commandBuffer, 0, CMD_BUFFER_SIZE);
        } else {
            commandBuffer[bufferIndex++] = c;
        }
    }
}