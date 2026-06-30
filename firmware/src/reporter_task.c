#include "pipeline_types.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>

#define REPORT_PERIOD_MS  500
#define BUF_LEN           64

void reporter_task(void *arg)
{
    (void)arg;

    char buf[BUF_LEN];
    TickType_t last_wake = xTaskGetTickCount();
    pipeline_state_t snap;

    for (;;) {
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(REPORT_PERIOD_MS));

        if (xSemaphoreTake(g_state_mutex, pdMS_TO_TICKS(5)) != pdTRUE) {
            continue;
        }
        snap = g_pipeline_state;   /* copy under mutex, then release fast */
        xSemaphoreGive(g_state_mutex);

        /* Format: tick_ms,filtered,min,max,count\r\n
         * Host monitor parses on the comma delimiter. */
        int n = snprintf(buf, BUF_LEN, "%lu,%u,%u,%u,%lu\r\n",
                         (unsigned long)(snap.last_updated),
                         snap.filtered,
                         snap.min,
                         snap.max,
                         (unsigned long)(snap.sample_count));

        if (n > 0 && n < BUF_LEN) {
            uart_write(buf, (size_t)n);
        }
    }
}
