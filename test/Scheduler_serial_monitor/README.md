# Arduino Scheduler Test

This folder contains the code and resources for testing a dynamic task scheduler on an Arduino. The scheduler allows you to add, remove, inspect, and control tasks via serial commands.


---

## **Getting Started**

### **1. Prerequisites**
- Arduino IDE installed.
- Arduino board connected to your computer.
- Serial Monitor (9600 baud).

### **2. Upload the Code**
1. Open the `test.ino` file in the Arduino IDE.
2. Ensure the `scheduler.h` and `scheduler.c` files are in the same folder.
3. Upload the sketch to your Arduino.

### **3. Using the Serial Monitor**
Open the Serial Monitor (`9600` baud) to interact with the scheduler. Supported commands:

#### **Add a Task**
- **Syntax:** `exec <taskname> [-p <priority>] [-t <interval>]`
- **Examples:**
exec task1 -t 1000 // Run task1 every 1 second
exec task2 -p 5 -t 2000 // Run task2 every 2 seconds with priority 5

#### **Remove a Task**
- **Syntax:** `halt <taskname>`
- **Example:**
halt task1

#### **Inspect Tasks**
- **Syntax:** `inspect`
- **Description:** Pauses execution and displays details of all scheduled tasks.

#### **Resume Execution**
- **Syntax:** `cont`
- **Description:** Resumes task execution after inspection.

---

## **Task Definitions**
Tasks are defined in `test.ino`. Example tasks (`task1`, `task2`, etc.) are provided for testing. You can add your own tasks by:
1. Defining the task function:
 ```cpp
 void myTask() {
     static unsigned long last = 0;
     if (millis() - last >= 1000) {
         Serial.println("My Task running");
         last = millis();
     }
 }
2. Registering the task in setup():
scheduler_register_task("mytask", myTask);
