/*
Deadlock : A deadlock occurs when two or more tasks are waiting forever for resources held by each other — so none of them can continue.
Example:
Task A locks Resource 1 → waits for Resource 2
Task B locks Resource 2 → waits for Resource 1
*/

/*
About below example:
>> TaskA takes Mutex1 and waits for Mutex2 (which TaskB owns).
>> TaskB takes Mutex2 and waits for Mutex1 (which TaskA owns).
>> Neither releases its mutex because they’re both blocked — classic deadlock.
*/


/*
How to Fix It ??
Use a consistent locking order for all tasks.
👉 If every task always locks Mutex1 before Mutex2, the circular wait never occurs.

---------------------------------------------------------------------------------------------------
Lets understand actually what is going on with below example:
TaskA locks mutex1, then tries to lock mutex2.
TaskB locks mutex2, then tries to lock mutex1.
So if:
TaskA takes mutex1,
TaskB takes mutex2,
Each task now waits for the other mutex → circular wait happens → system hangs.
---------------------------------------------------------------------------------------------------

If both tasks always take mutexes in the same sequence, say:
Always lock mutex1 first, then mutex2.
Then:
1) Even if both tasks try to run at the same time,
2) One of them will successfully take mutex1,
3) The other will wait until mutex1 is released,
4) Then it will proceed in the same safe order.
*/


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "DEADLOCK";

// Mutexes handles
SemaphoreHandle_t       mutex1;
SemaphoreHandle_t       mutex2;


//-------------------------------------------------------------------------------------------------
// Task A tries to take mutex1 then mutex2
void TaskA(void *pvParameters) {
    while (1) {
        // Take mutex1
        if (xSemaphoreTake(mutex1, portMAX_DELAY)) {
            ESP_LOGI(TAG, "TaskA: Took Mutex1");

            vTaskDelay(pdMS_TO_TICKS(100));  // Simulate processing

            ESP_LOGI(TAG, "TaskA: Trying to take Mutex2...");

            // Try to take mutex2
            if (xSemaphoreTake(mutex2, portMAX_DELAY)) {
                ESP_LOGI(TAG, "TaskA: Took Mutex2");
                ESP_LOGI(TAG, "TaskA: Working in critical section");

                // Release mutex2
                xSemaphoreGive(mutex2);
            }


            // Release mutex1
            xSemaphoreGive(mutex1);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}



//-------------------------------------------------------------------------------------------------
// Task B tries to take mutex2 then mutex1 -  Order is reversed compared to TaskA
void TaskB(void *pvParameters) {
    while (1) {
        // Take mutex2
        if (xSemaphoreTake(mutex2, portMAX_DELAY)) {
            ESP_LOGI(TAG, "TaskB: Took Mutex2");

            vTaskDelay(pdMS_TO_TICKS(100));  // Simulate processing

            ESP_LOGI(TAG, "TaskB: Trying to take Mutex1...");

            // Try to take mutex1
            if (xSemaphoreTake(mutex1, portMAX_DELAY)) {
                ESP_LOGI(TAG, "TaskB: Took Mutex1");
                ESP_LOGI(TAG, "TaskB: Working in critical section");

                // Release mutex1
                xSemaphoreGive(mutex1);
            }
            // Release mutex2
            xSemaphoreGive(mutex2);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


//-------------------------------------------------------------------------------------------------
void app_main(void) {
    ESP_LOGI(TAG, "Starting Deadlock Example");

    // Create mutexes
    mutex1 = xSemaphoreCreateMutex();
    mutex2 = xSemaphoreCreateMutex();

    if (mutex1 == NULL || mutex2 == NULL) {
        ESP_LOGE(TAG, "Failed to create mutexes");
        return;
    }

    // Create tasks 
    xTaskCreate(TaskA, "TaskA", 4096, NULL, 2, NULL);
    xTaskCreate(TaskB, "TaskB", 4096, NULL, 2, NULL);
}
//-------------------------------------------------------------------------------------------------