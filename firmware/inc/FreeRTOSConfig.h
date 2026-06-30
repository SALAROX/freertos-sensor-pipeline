/*
 * FreeRTOSConfig.h — STM32F411, 16 MHz HSI (no PLL; adjust if you've
 * configured the PLL and are running at 100 MHz).
 *
 * The interrupt priority values below are the most likely thing to need
 * changing if you port this to a different project. Short version:
 *   - configMAX_SYSCALL_INTERRUPT_PRIORITY must be set in the format the
 *     NVIC actually uses (top nibble, 4-bit priority on Cortex-M4).
 *   - Any ISR that calls a FreeRTOS API must have a priority value
 *     >= configMAX_SYSCALL_INTERRUPT_PRIORITY numerically (lower urgency).
 *   - 0 (highest possible) must never be used for FreeRTOS-aware ISRs.
 * Getting this wrong gives you a hard fault in the best case, silent
 * corruption in the worst. The values here (priority 5 for syscall limit,
 * ISRs at priority 6) leave headroom at the top for anything genuinely
 * time-critical that doesn't touch the RTOS.
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      16000000UL
#define configTICK_RATE_HZ                      1000
#define configMAX_PRIORITIES                    8
#define configMINIMAL_STACK_SIZE                128  /* words, not bytes */
#define configMAX_TASK_NAME_LEN                 12
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_COUNTING_SEMAPHORES           0
#define configQUEUE_REGISTRY_SIZE               4
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIME_SLICING                  1
#define configUSE_NEWLIB_REENTRANT              0
#define configENABLE_BACKWARD_COMPATIBILITY     0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 0

/* Memory */
#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   (20 * 1024)  /* 20 KB of 128 KB SRAM */
#define configAPPLICATION_ALLOCATED_HEAP        0

/* Hook functions */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCHECK_FOR_STACK_OVERFLOW          2  /* method 2: fills stack with known pattern */
#define configUSE_MALLOC_FAILED_HOOK            1

/* Runtime stats — requires a hardware timer; disabled here to keep the
 * config self-contained. Enable if you wire up a 32-bit timer counter. */
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_TRACE_FACILITY                0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/* Software timers */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                4
#define configTIMER_TASK_STACK_DEPTH            256  /* words */

/* Cortex-M interrupt priority config.
 * 0x50 = priority 5 shifted into the top nibble (4-bit priority field). */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY      15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY         (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << 4)
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << 4)

/* Assert: spins in place so a debugger can catch it */
#define configASSERT(x) do { if (!(x)) { __asm volatile("bkpt #0"); for(;;); } } while(0)

/* FreeRTOS API subsets to include */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  0
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          0
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xEventGroupSetBitFromISR        0
#define INCLUDE_xTimerPendFunctionCall          0
#define INCLUDE_xTaskAbortDelay                 0
#define INCLUDE_xTaskGetHandle                  0

/* Map FreeRTOS port Cortex-M handlers to their CMSIS names */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
