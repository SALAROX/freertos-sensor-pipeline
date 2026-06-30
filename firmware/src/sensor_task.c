#include "pipeline_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Sensor read period. vTaskDelayUntil keeps this accurate regardless of
 * how long the actual read takes — important if you swap the stub out for
 * a real I2C read that might occasionally stretch. */
#define SENSOR_PERIOD_MS  50

/* ---- Stub: replace this block with a real sensor read ----
 *
 * Generates a sawtooth so the moving average in processing_task has
 * something non-trivial to work with. If you have an I2C sensor wired up,
 * swap this function body for your actual read (e.g. i2c_read() from the
 * other project in this repo, then extract the relevant bytes).
 */
static uint16_t read_sensor(void)
{
    static uint16_t val = 0;
    val = (uint16_t)((val + 13) & 0x03FF);  /* 10-bit sawtooth */
    return val;
}
/* ---------------------------------------------------------- */

void sensor_task(void *arg)
{
    (void)arg;

    TickType_t last_wake = xTaskGetTickCount();
    sensor_sample_t sample;

    for (;;) {
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(SENSOR_PERIOD_MS));

        sample.raw  = read_sensor();
        sample.tick = xTaskGetTickCount();

        /* Non-blocking post — if the queue is full the processing task
         * is backed up, which shouldn't happen at these rates but we
         * drop rather than block here to preserve the timing. */
        xQueueSend(g_sensor_queue, &sample, 0);
    }
}
