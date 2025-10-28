#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char* TAG = "EX2";
static QueueHandle_t q;

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

void consumer_task(void *pv) {
    int rx = 0;
    while (1) {
        if (xQueueReceive(q, &rx, portMAX_DELAY) == pdPASS) {
            ESP_LOGI(TAG, "Got value: %d", rx);
        }
    }
}

void app_main(void) {
    q = xQueueCreate(10, sizeof(int)); // depth 10
    configASSERT(q != NULL);

    xTaskCreate(producer_task, "producer", 2048, NULL, 5, NULL);
    xTaskCreate(consumer_task, "consumer", 2048, NULL, 5, NULL);
}
