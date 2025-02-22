#include <Arduino.h>
#include <string.h>

#define MAX_TASKS 10
#define CMD_BUFFER_SIZE 64
#define MAX_REGISTERED_TASKS 10

typedef void (*TaskFunction)();

// Task structure for scheduler
typedef struct {
    TaskFunction function;
    char name[20];
    unsigned long interval;
    unsigned long lastRun;
    int priority;
    bool active;
} ScheduledTask;

// Structure for registered tasks
typedef struct {
    char name[20];
    TaskFunction function;
} RegisteredTask;

// Global variables
ScheduledTask taskList[MAX_TASKS];
RegisteredTask registeredTasks[MAX_REGISTERED_TASKS];
int taskCount = 0;
int registeredTaskCount = 0;
bool isPaused = false;
char commandBuffer[CMD_BUFFER_SIZE];

// Initialize scheduler
void scheduler_init() {
    taskCount = 0;
    isPaused = false;
}

// Register a task
void scheduler_register_task(const char* name, TaskFunction function) {
    if (registeredTaskCount >= MAX_REGISTERED_TASKS) return;
    strncpy(registeredTasks[registeredTaskCount].name, name, 19);
    registeredTasks[registeredTaskCount].function = function;
    registeredTaskCount++;
}

// Add a task to the scheduler
void scheduler_add_task(const char* name, int priority, unsigned long interval) {
    if (taskCount >= MAX_TASKS) return;

    // Find the task function
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

    // Priority management
    if (priority == -1) {
        int minPriority = 0;
        for (int i = 0; i < taskCount; i++) {
            if (taskList[i].priority < minPriority) minPriority = taskList[i].priority;
        }
        priority = minPriority - 1;
    }

    // Insert in priority queue (higher priority first)
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

// Remove a task from the scheduler
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

// Run the scheduler
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

// Inspect tasks
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

// Handle serial commands
void scheduler_handle_command() {
    static int bufferIndex = 0;

    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n' || bufferIndex >= CMD_BUFFER_SIZE-1) {
            commandBuffer[bufferIndex] = '\0';
            bufferIndex = 0;

            // Parse command
            char cmd[10];
            char taskName[20];
            int priority = -1;
            unsigned long interval = 0;

            sscanf(commandBuffer, "%s", cmd);

            if (strcmp(cmd, "exec") == 0) {
                char* ptr = commandBuffer + 4;
                sscanf(ptr, "%s", taskName);
                
                // Look for parameters
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

// Example tasks
void task1() {
    static unsigned long last = 0;
    if (millis() - last >= 4000) {
        Serial.println("hello1 4");
        last = millis();
    }
}

void task2() {
    static unsigned long last = 0;
    if (millis() - last >= 2000) {
        Serial.println("hello2 2");
        last = millis();
    }
}

void task3() {
    static unsigned long last = 0;
    if (millis() - last >= 3000) {
        Serial.println("hello3 3");
        last = millis();
    }
}

void task4() {
    static unsigned long last = 0;
    if (millis() - last >= 4000) {
        Serial.println("hello4 4");
        last = millis();
    }
}

void task5() {
    static unsigned long last = 0;
    if (millis() - last >= 5000) {
        Serial.println("hello5 5");
        last = millis();
    }
}

void task6() {
    static unsigned long last = 0;
    if (millis() - last >= 1000) {
        Serial.println("hello6 no delay");
        last = millis();
    }
}

void setup() {
    Serial.begin(9600);
    scheduler_init();
    
    // Register your tasks
    scheduler_register_task("task1", task1);
    scheduler_register_task("task2", task2);
    scheduler_register_task("task3", task3);
    scheduler_register_task("task4", task4);
    scheduler_register_task("task5", task5);
    scheduler_register_task("task6", task6);
    
    Serial.println("Scheduler ready");
}

void loop() {
    scheduler_handle_command();
    scheduler_run();
    delay(50); // Small delay to prevent watchdog triggers
}