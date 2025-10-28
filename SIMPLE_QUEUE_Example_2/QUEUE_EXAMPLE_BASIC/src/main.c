#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"          //TASK FUNCTIONS LIB
#include "freertos/queue.h"         //QUEUE FUNCTIONS LIB
#include "esp_log.h"


/*
---------------------------------------------------------------------------------------------------
Why QUEUE is needed in RTOS?
Queues are used in RTOS to facilitate safe and efficient communication between tasks.
They provide a thread-safe (Thread safe means way to prevent task switching during critical operations) way to send and receive data, allowing tasks to exchange information without the need for complex synchronization mechanisms. 
Queues help in decoupling tasks, enabling them to operate independently while still being able to share data. 
This is particularly important in real-time systems where timing and responsiveness are critical.

>>Queue Operations Atomic:
1) Operations cannot be interrupted by other tasks or ISRs
2) They either complete fully or not at all
3) No partial updates are possible
---------------------------------------------------------------------------------------------------
*/


static const char* TAG = "EX2";
static QueueHandle_t q;


//---------------------------------------------------------------------------------------------------
void producer_task(void *pv) {
    int value = 0;
    while (1) {
        value++;
        if (xQueueSend(q, &value, pdMS_TO_TICKS(10)) == pdPASS) {
            // sent
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
//---------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------
void consumer_task(void *pv) {
    int rx = 0;
    while (1) {
        if (xQueueReceive(q, &rx, portMAX_DELAY) == pdPASS) {
            ESP_LOGI(TAG, "Got value: %d", rx);
        }
    }
}
//---------------------------------------------------------------------------------------------------



void app_main(void) 
{

    //------------------------------------------------------------------------------------------------
    //xQueueCreate Function to create a queue
    //Parameters: uxQueueLength: The maximum number of items the queue can hold at any one time.
    //uxItemSize: The size, in bytes, of each item that can be stored in the queue.
    //Returns: If the queue is created successfully, a handle to the queue is returned. If the queue cannot be created, NULL is returned.
    q = xQueueCreate(10, sizeof(int));          
    configASSERT(q != NULL);
    //------------------------------------------------------------------------------------------------


    //Create Producer and Consumer Tasks
    xTaskCreate(producer_task, "producer", 2048, NULL, 5, NULL);
    xTaskCreate(consumer_task, "consumer", 2048, NULL, 5, NULL);
}
