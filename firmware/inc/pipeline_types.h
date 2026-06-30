#ifndef PIPELINE_TYPES_H
#define PIPELINE_TYPES_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Raw sample posted from sensor_task to processing_task via queue */
typedef struct {
    uint16_t    raw;
    TickType_t  tick;   /* FreeRTOS tick at time of read */
} sensor_sample_t;

/* Processed state shared between processing_task (writer)
 * and reporter_task (reader) via mutex */
typedef struct {
    uint16_t    filtered;
    uint16_t    min;
    uint16_t    max;
    uint32_t    sample_count;
    TickType_t  last_updated;
} pipeline_state_t;

/* Queue and mutex handles — defined in main.c, extern here */
extern QueueHandle_t  g_sensor_queue;
extern SemaphoreHandle_t g_state_mutex;
extern pipeline_state_t  g_pipeline_state;

#endif /* PIPELINE_TYPES_H */
