/*
 * startup.c — same as the bare-metal project; see notes there.
 */

#include <stdint.h>

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack;
extern int main(void);

void Reset_Handler(void);
static void Default_Handler(void);

void NMI_Handler(void)          __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));

/* SVC/PendSV/SysTick are NOT aliased to Default_Handler — FreeRTOS
 * defines these via the macros in FreeRTOSConfig.h */
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))&_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0, 0, 0, 0,
    SVC_Handler,
    0, 0,
    PendSV_Handler,
    SysTick_Handler,
    /* Peripheral IRQs — only USART2 is used in this project */
    [38 + 16] = USART2_IRQHandler,
};

void Reset_Handler(void)
{
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) { *dst++ = *src++; }

    dst = &_sbss;
    while (dst < &_ebss) { *dst++ = 0; }

    main();
    for (;;) {}
}

static void Default_Handler(void)
{
    for (;;) {}
}
