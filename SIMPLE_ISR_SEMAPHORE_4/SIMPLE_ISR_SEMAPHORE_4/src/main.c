/*
About Example: 

Why ISR is critical?
The interrupt needs to be handled immediately.ISRs must run immediately and return — they cannot wait

But you can’t do much inside an ISR because:
>> ISRs must be very short (they interrupt everything else, so you may miss other interrupts).
>> You can’t use normal FreeRTOS APIs that might block or take time. Eg: vTaskDelay, xSemaphoreTake without FromISR, etc are wait type - Don't use it.
>> Doing long operations (like printing, I/O, or delays) inside an ISR will crash or hang your system.
>> ISR is on RAM, interrupt/IRAM stacks are limited. Heavy computation or deep recursion can overflow stack.
>> long ISR prevents the scheduler tick and other tasks from running, possibly triggering watchdog resets.

How It Works??
The ISR “gives” a semaphore when the event happens.
It’s safe and fast — only a few CPU cycles.
A FreeRTOS task (running in normal context) “takes” the semaphore and performs the heavy work — like printing logs, toggling LEDs, or processing data.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"        // For SemaphoreHandle_t
#include "driver/gpio.h"            // For GPIO functions and types
#include "esp_log.h"



#define LED_PIN   GPIO_NUM_2        // Built-in LED
#define BTN_PIN   GPIO_NUM_0        // Boot button on most ESP32 boards


// Semaphore handle
static SemaphoreHandle_t xButtonSemaphore;


//-------------------------------------------------------------------------------------------------
// Interrupt handler (runs in ISR context)
// IRAM_ATTR ensures the function is placed in IRAM for fast execution.
// Note : printf/malloc use locks/heap state that are not ISR-safe — do not call them from ISRs
//      : ISRs cannot block or sleep. Blocking APIs (vTaskDelay, xSemaphoreTake without FromISR, etc are wait type - Don't use it.) — ISRs must never wait.
static void IRAM_ATTR button_isr_handler(void *arg) 
{

    //Variable to check if a higher priority task was woken
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /*
    Function: xSemaphoreGiveFromISR() - "Give" the semaphore to unblock the task
    Parameters:
    >> xButtonSemaphore: The semaphore handle to give.
    >> &xHigherPriorityTaskWoken: Pointer to a variable that will be set to pdTRUE if giving the semaphore caused a higher priority task to unblock.
       it means that if the unblocked task has a higher priority than the currently running task, a context switch should be performed at the end of the ISR.
    
    Why we do this?
    >> When the button is pressed, we need to run relevant high priority task, but it won't start until the ISR gives the semaphore.
    >> This means that the running task is less important, and once the ISR routine is done, the kernel will automatically go back to the previous task.
    >> To prevent this from happening, we have to manually switch the context of the task.

    Working:
    High Priority Task Running
                 ↓
    Button Press → ISR Interrupts → button_isr_handler() runs
                 ↓
    xSemaphoreGiveFromISR() called
                 ↓
    If waiting task has HIGHER priority than current task:
    → xHigherPriorityTaskWoken set to pdTRUE
    → portYIELD_FROM_ISR() forces context switch
    */

    xSemaphoreGiveFromISR(xButtonSemaphore, &xHigherPriorityTaskWoken);


    if (xHigherPriorityTaskWoken) {

        // Request a context switch to the higher priority task
        //Forcefully yield to the higher priority task that was woken by the semaphore give
        portYIELD_FROM_ISR();
    }
}
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
// Task waits for semaphore and toggles LED
void button_task(void *pvParameters) {
    int led_state = 0;
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        if (xSemaphoreTake(xButtonSemaphore, portMAX_DELAY)) {
            led_state = !led_state;
            gpio_set_level(LED_PIN, led_state);
            printf("Button pressed! LED is now: %s\n", led_state ? "ON" : "OFF");
        }
    }
}
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
void app_main(void) {

    //****************************************************
    // Create binary semaphore
    xButtonSemaphore = xSemaphoreCreateBinary();
    if (xButtonSemaphore == NULL) {
        //NOTE : PRINTF() is used here for simplicity. In production code, consider using ESP_LOGE() for error logging.
        printf("Failed to create semaphore!\n");
        return;
    }
    //****************************************************


    //****************************************************
    // Configure button GPIO as input with pull-up and interrupt on falling edge (For ISR)
    gpio_config_t btn_conf = {
        .pin_bit_mask = 1ULL << BTN_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE              // Trigger on falling edge
    };

    //Funtion call to configure the GPIO pin
    //gpio_config applies configuration only to the pins whose bits are set in .pin_bit_mask. Other pins keep their current configuration.
    gpio_config(&btn_conf);
    //****************************************************


    //****************************************************
    /*
    Note : Install ISR service only once for all GPIOs, All GPIO interrupts will share the same ISR service.
    
    Function: gpio_install_isr_service()
    >> This function sets up the GPIO interrupt service routine (ISR) handler for the GPIO driver. 
    >> It allows you to register individual interrupt handlers for specific GPIO pins.
    
    Parameters: Here value 0 means default interrupt allocation flags.
                Parameters like ESP_INTR_FLAG_LEVEL1, ESP_INTR_FLAG_IRAM, etc., can be used to customize the interrupt behavior.
    >> ESP_INTR_FLAG_LEVEL1: Sets the interrupt level to 1. Level 1 interrupts can be masked by higher-level interrupts.
    >> ESP_INTR_FLAG_IRAM: Places the ISR in IRAM for faster execution.
    >> 0 means no special flags are set. 0 means default behavior without any special configurations.
    */
    gpio_install_isr_service(0);

    /*
    Note : Attach the button ISR handler to the button GPIO pin.
    Function: gpio_isr_handler_add()
    >> This function registers an interrupt handler for a specific GPIO pin.

    Parameters:
    >> gpio_num: The GPIO pin number to which the ISR handler is attached.
    >> isr_handler: The function to be called when the interrupt occurs.
    >> args: A pointer to user-defined data that will be passed to the ISR handler when it is called.
    */
    gpio_isr_handler_add(BTN_PIN, button_isr_handler, NULL);
    //****************************************************


    //****************************************************
    // Create task that waits for button press
    xTaskCreate(button_task, "ButtonTask", 2048, NULL, 5, NULL);
    //****************************************************

}

