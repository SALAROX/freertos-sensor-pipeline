#include "stm32f411_regs.h"
#include "uart.h"
#include "pipeline_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/* Shared objects — defined here, extern'd in pipeline_types.h */
QueueHandle_t    g_sensor_queue;
SemaphoreHandle_t g_state_mutex;
pipeline_state_t  g_pipeline_state;

/* Task entry points */
void sensor_task(void *arg);
void processing_task(void *arg);
void reporter_task(void *arg);

/* Stack sizes (words). These are generous for an F411 with 128 KB SRAM;
 * check uxTaskGetStackHighWaterMark() during development to trim if needed. */
#define SENSOR_STACK      256
#define PROCESSING_STACK  384   /* slightly more; uses snprintf indirectly */
#define REPORTER_STACK    384

#define SENSOR_PRIORITY      (tskIDLE_PRIORITY + 3)
#define PROCESSING_PRIORITY  (tskIDLE_PRIORITY + 2)
#define REPORTER_PRIORITY    (tskIDLE_PRIORITY + 1)

#define SENSOR_QUEUE_LEN  8     /* headroom for burst; normal fill is ~1 */

/* LED blink timer callback — PC13 on Nucleo-F411RE */
static void led_timer_cb(TimerHandle_t xTimer)
{
    (void)xTimer;
    GPIOC->ODR ^= (1UL << 13);
}

static void led_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER &= ~(3UL << (13 * 2));
    GPIOC->MODER |=  (1UL << (13 * 2));   /* output */
}

int main(void)
{
    /* Interrupt priority grouping: 4 bits preemption, 0 sub-priority.
     * Must be set before creating FreeRTOS objects — some ports assert on
     * mismatch. VECTKEY is required in bits [31:16] or the write is ignored. */
    SCB_AIRCR = SCB_AIRCR_PRIGROUP_4_NONE;

    led_init();
    uart_init(115200);

    g_pipeline_state.min = UINT16_MAX;   /* so first real sample wins */
    g_pipeline_state.max = 0;

    g_sensor_queue = xQueueCreate(SENSOR_QUEUE_LEN, sizeof(sensor_sample_t));
    configASSERT(g_sensor_queue != NULL);

    g_state_mutex = xSemaphoreCreateMutex();
    configASSERT(g_state_mutex != NULL);

    /* 1 Hz LED blink as liveness indicator */
    TimerHandle_t led_timer = xTimerCreate("led", pdMS_TO_TICKS(500),
                                            pdTRUE, NULL, led_timer_cb);
    configASSERT(led_timer != NULL);
    xTimerStart(led_timer, 0);

    xTaskCreate(sensor_task,     "sensor",  SENSOR_STACK,     NULL, SENSOR_PRIORITY,     NULL);
    xTaskCreate(processing_task, "process", PROCESSING_STACK, NULL, PROCESSING_PRIORITY, NULL);
    xTaskCreate(reporter_task,   "report",  REPORTER_STACK,   NULL, REPORTER_PRIORITY,   NULL);

    vTaskStartScheduler();

    /* Should never reach here — if it does, heap is probably exhausted */
    for (;;) {}
}

/* FreeRTOS hook for malloc failure */
void vApplicationMallocFailedHook(void)
{
    configASSERT(0);
}

/* FreeRTOS hook for stack overflow */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    configASSERT(0);
}
