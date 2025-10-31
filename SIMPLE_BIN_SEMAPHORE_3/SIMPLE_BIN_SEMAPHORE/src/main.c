//Binary semaphore example using FreeRTOS
//One Task (Task A) gives the semaphore every second
//Two other tasks (Task B and Task C) wait to take the semaphore and print a message when they receive it


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"       // Include semaphore header

/*
What is Semaphore?
A semaphore is a synchronization primitive used to manage access to shared resources in concurrent programming
In simple words, a semaphore is like a signal that allows tasks to communicate with each other and coordinate their actions.

FAQ : What is the difference between a binary semaphore and a queue?
Ans : A binary semaphore is a synchronization primitive that can take only two states: "available" (1) and "unavailable" (0). It is used to signal between tasks or to protect shared resources, allowing only one task to access a resource at a time.
A queue, on the other hand, is a data structure that allows multiple items to be stored and retrieved in a first-in-first-out (FIFO) manner. Queues are used for inter-task communication, where tasks can send and receive messages or data.

Realtime example:
Imagine a scenario where two workers (tasks) need to use a single tool (shared resource). A binary semaphore acts like a "tool available" sign. 
When one worker is using the tool, the sign is set to "unavailable," preventing the other worker from using it until the first worker is done and sets the sign back to "available."
In RTOS this tool means a shared resource like a printer, a sensor, or a communication channel.

What happens if both workers try to use the tool at the same time without a semaphore?
In this case, both workers might try to use the tool simultaneously, leading to conflicts, errors, or even damage to the tool. 
The semaphore ensures that only one worker can use the tool at a time, preventing such issues.
*/



// Declare a binary semaphore handle
SemaphoreHandle_t xSemaphore;



//-------------------------------------------------------------------------------------------------
//TASK-A : Give the semaphore every second
void taskA(void *pvParameters) {
    while (1) {

        //xSemaphoreGive() = Function to give the semaphore
        //If the semaphore is already given, this function has no effect.
        printf("Task A: Giving semaphore\n");
        xSemaphoreGive(xSemaphore);           // Signal the semaphore

        vTaskDelay(pdMS_TO_TICKS(1000));      // Wait 1 second
    }
}
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//taskB : Wait to take the semaphore
void taskB(void *pvParameters) {
    while (1) {
        // Wait indefinitely until Task A gives the semaphore
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
            printf("Task B: Received semaphore!\n");
        }
    }
}
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//taskC : Wait to take the semaphore
void taskC(void *pvParameters) {
    while (1) {
        // Wait indefinitely until Task A gives the semaphore
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
            printf("Task C: Received semaphore!\n");
        }
    }
}
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
void app_main(void) {
    //Create a binary semaphore (initially empty)
    xSemaphore = xSemaphoreCreateBinary();

    if (xSemaphore == NULL) {
        printf("Failed to create semaphore\n");
        return;
    }


    // Create the two tasks
    xTaskCreate(taskA, "TaskA", 2048, NULL, 2, NULL);
    xTaskCreate(taskB, "TaskB", 2048, NULL, 1, NULL);
    xTaskCreate(taskC, "TaskC", 2048, NULL, 1, NULL);
}
//-------------------------------------------------------------------------------------------------