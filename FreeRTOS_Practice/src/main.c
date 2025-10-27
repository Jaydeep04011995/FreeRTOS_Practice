#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

void task1(void *pv) {
  while (1) {
    ESP_LOGI("TASK1", "Running...");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void task2(void *pv) {
  while (1) {
    ESP_LOGI("TASK2", "Running...");
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

extern "C" void app_main(void) {
  xTaskCreate(task1, "task1", 2048, NULL, 1, NULL);
  xTaskCreate(task2, "task2", 2048, NULL, 1, NULL);
}
