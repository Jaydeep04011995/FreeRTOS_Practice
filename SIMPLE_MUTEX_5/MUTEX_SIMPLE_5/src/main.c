/*
Mutex example:
A mutex is like a single key to a public restroom.
Only one person (thread) can have the key at a time.
That person must lock the door to enter and unlock it to exit.
They must also be the one to return the key; another person can't take it from them. 
This ensures that the shared resource (the restroom) is used exclusively by one thread at a time, preventing any conflict or corruption.

OWNERSHIP MODEL OF MUTEX:
Mutex has ownership, meaning only the thread that acquires (locks) the mutex is allowed to release (unlock) it. 
This ownership model ensures that only one thread can access a shared resource at a time and prevents other threads from unlocking a mutex they don't own. 

Tips:
1) Always release (xSemaphoreGive) the mutex after use.
2) Use timeouts instead of portMAX_DELAY in large systems to avoid permanent blocking.
3) For multiple shared resources, use one mutex per resource.


Note: Difference between binary semaphore and mutex:
1) A binary semaphore is a signaling mechanism that can be used for synchronization between tasks or between an interrupt and a task.
   On the other hand, Mutex has ownership and provides a lock for protecting shared resources.
2) Any task can give (release) a binary semaphore, whereas only the task that took (acquired) a mutex can give (release) it.
3) Mutex cannot be used in ISRs, while binary semaphores can be used in ISRs.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"            // For SemaphoreHandle_t and mutex functions


// Global mutex and shared resource
SemaphoreHandle_t xMutex;
int sharedCounter = 0;                  // Shared resource


//-------------------------------------------------------------------------------------------------
// Task A function : increments sharedCounter (Note Shared counter is shared resource)
void TaskA(void *pvParameters) {
    while (1) {
        // Try to take the mutex before accessing the shared resource
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            sharedCounter++;
            printf("TaskA incremented counter to %d\n", sharedCounter);

            // Release the mutex after done
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// Task B function : increments sharedCounter
void TaskB(void *pvParameters) {
    while (1) {

        // Try to take the mutex before accessing the shared resource
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            sharedCounter++;
            printf("TaskB incremented counter to %d\n", sharedCounter);

            // Release the mutex after done
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(700));
    }
}
//-------------------------------------------------------------------------------------------------



// Main application entry point
void app_main(void) {

    // Create the mutex before starting tasks
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        printf("Failed to create mutex\n");
        return;
    }

    // Create tasks
    xTaskCreate(TaskA, "TaskA", 2048, NULL, 2, NULL);
    xTaskCreate(TaskB, "TaskB", 2048, NULL, 2, NULL);
}
