#include "pipeline_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define FILTER_LEN  16

/* Fixed-point moving average. Using a power-of-2 length so the division
 * is a shift — avoids pulling in the soft-float division on Cortex-M4
 * when FPU is not enabled in this build. */
static uint16_t moving_average(uint16_t new_val)
{
    static uint32_t buf[FILTER_LEN];
    static uint8_t  idx = 0;
    static uint32_t acc = 0;

    acc       -= buf[idx];
    buf[idx]   = new_val;
    acc       += new_val;
    idx        = (idx + 1) & (FILTER_LEN - 1);

    return (uint16_t)(acc >> 4);  /* >> 4 == / 16 */
}

void processing_task(void *arg)
{
    (void)arg;

    sensor_sample_t sample;

    for (;;) {
        /* Block indefinitely waiting for samples — this task has nothing
         * to do between arrivals */
        if (xQueueReceive(g_sensor_queue, &sample, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        uint16_t filtered = moving_average(sample.raw);

        if (xSemaphoreTake(g_state_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_pipeline_state.filtered = filtered;
            g_pipeline_state.last_updated = sample.tick;
            g_pipeline_state.sample_count++;

            if (filtered < g_pipeline_state.min) {
                g_pipeline_state.min = filtered;
            }
            if (filtered > g_pipeline_state.max) {
                g_pipeline_state.max = filtered;
            }

            xSemaphoreGive(g_state_mutex);
        }
        /* If mutex times out, skip this update — reporter will get
         * the previous valid state on its next cycle. */
    }
}
