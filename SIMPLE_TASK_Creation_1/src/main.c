//Example -1 Create Task using xTaskCreate()

//Lib Required
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

//Note
/*
---------------------------------------------------------------------------------------------------
Function Prototype:

#1 : Add Task to FreeRTOS Scheduler:
BaseType_t xTaskCreate( TaskFunction_t pvTaskCode,
                        const char * const pcName,
                        configSTACK_DEPTH_TYPE usStackDepth,
                        void *pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t *pxCreatedTask );

//Parameters
1) TaskFunction_t: typedef void (*TaskFunction_t)( void * ); — pointer to task function.

2) const char * const pcName: A descriptive name for the task. This is mainly used for debugging purposes.
pointer to a null-terminated name string for the task (can be NULL).

3) configSTACK_DEPTH_TYPE usStackDepth: The size of the task stack specified as the number of variables the stack can hold—not the number of bytes. For example, if the stack is 32 bits wide and usStackDepth is defined as 100, then 400 bytes will be allocated for the stack.
stack size for the task (number of stack words; exact unit is config-dependent/port-dependent).
Note: This is the stack size in words, not bytes. The actual byte size will depend on the architecture (e.g., 4 bytes per word on a 32-bit architecture).

4) void *pvParameters: Pointer that will be used as the parameter for the task being created.
parameter passed into the task function (can be NULL).

5) UBaseType_t uxPriority: The priority at which the task should run. 
Priorities are specified as numbers, with higher numbers indicating higher priorities.
---------------------------------------------------------------------------------------------------

USe of:
pdMS_TO_TICKS() - Convert milliseconds to ticks
>> convert a time in milliseconds to FreeRTOS ticks for use with tick-based APIs (e.g., vTaskDelay).


FAQ: About stack size allocation for tasks in FreeRTOS
>> If stack is insufficient you get stack overflow -> memory corruption, corrupted variables/return addresses, crashes or hard faults (undefined behaviour).

>> If configCHECK_FOR_STACK_OVERFLOW is enabled, FreeRTOS will detect many overflows and call vApplicationStackOverflowHook(). You should implement that hook to catch/handle the condition.

>> Rule of Thumb for stack size allocation for Tasks:
   - Simple tasks (e.g., LED blink): 128 to 256 words
   - Moderate tasks (e.g., sensor reading, communication): 512 to 1024 words
   - Complex tasks (e.g., networking, file systems): 2048 words or more

>> Check remaining stack (high water mark) for a task during testing:
UBaseType_t hi = uxTaskGetStackHighWaterMark(NULL); // returns words remaining
ESP_LOGI("STACK", "High water mark: %u words (%u bytes)", hi, (unsigned)(hi * sizeof(StackType_t)));
*/
//---------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------
//Create Task -1
void task1(void *pv) {
  while (1) {
    ESP_LOGI("TASK1", "Running...");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
//---------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------------
//Create Task - 2
void task2(void *pv) {
  while (1) {
    ESP_LOGI("TASK2", "Running...");
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
//---------------------------------------------------------------------------------------------------



//Main
void app_main(void) {
  xTaskCreate(task1, "task1", 2048, NULL, 1, NULL);
  xTaskCreate(task2, "task2", 2048, NULL, 1, NULL);
}
