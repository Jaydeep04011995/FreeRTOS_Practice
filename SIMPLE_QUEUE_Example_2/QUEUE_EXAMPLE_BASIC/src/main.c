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
        //-----------------------------------------------------------------------------------------
        /*
        Function : xQueueSend
        >> Description: This function is used to send an item to the back of a queue.
        >> Parameters: xQueue: The handle of the queue to which the item is being sent.
        >> pvItemToQueue: A pointer to the item that is to be placed on the queue.
        >> xTicksToWait: The maximum amount of time the task should block waiting for space to become available on the queue, should it already be full.
            It tells FreeRTOS how long the calling task is willing to wait if the queue is full (for xQueueSend) or empty (for xQueueReceive).
            If the queue has space right now: the call returns immediately and nothing is waited.
            If the queue is full and xTicksToWait > 0: the task is suspended (not busy‑waiting) and the scheduler runs other tasks; when space appears the task is resumed and the item is sent. 
            If the queue is full and xTicksToWait == 0: the call returns immediately with failure
        >> Returns: pdPASS if the item was successfully sent to the queue, otherwise errQUEUE_FULL.
            */
        //-----------------------------------------------------------------------------------------
        if (xQueueSend(q, &value, pdMS_TO_TICKS(10)) == pdPASS) {
            // sent sucessfully
        }
        else
        {
            //If the queue is full and xTicksToWait == 0: the call returns immediately with failure
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
//---------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------
void consumer_task(void *pv) {
    int rx = 0;
    while (1) {
        //-----------------------------------------------------------------------------------------
        /*
        Function : xQueueReceive
        >> Description: This function is used to receive an item from a queue.
        >> Parameters: xQueue: The handle of the queue from which the item is to be received.
        >> pvBuffer: A pointer to the buffer into which the received item will be copied.
        >> xTicksToWait: The maximum amount of time the task should block waiting for an item to be available on the queue, should it already be empty.
            It tells FreeRTOS how long the calling task is willing to wait if the queue is full (for xQueueSend) or empty (for xQueueReceive).
            If the queue has data right now: the call returns immediately and nothing is waited.
            If the queue is empty and xTicksToWait > 0: the task is suspended (not busy‑waiting) and the scheduler runs other tasks; when data appears the task is resumed and the item is received. 
            If the queue is empty and xTicksToWait == 0: the call returns immediately with failure
        >> Returns: pdPASS if an item was successfully received from the queue, otherwise errQUEUE_EMPTY.
        */
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
    q = xQueueCreate(10, sizeof(int));          //Depth = 10, because your send/receive queue function calls / pass pointers to int values in our example.
    configASSERT(q != NULL);
    //q is The handle of the queue to which the item is being sent / or receive.
    //------------------------------------------------------------------------------------------------


    //Create Producer and Consumer Tasks
    xTaskCreate(producer_task, "producer", 2048, NULL, 5, NULL);
    xTaskCreate(consumer_task, "consumer", 2048, NULL, 5, NULL);
}
