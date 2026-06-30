/*
 * stm32f411_regs.h
 * Register definitions for peripherals used in this project.
 * Covers RCC, GPIOA, USART2, and the SysTick/NVIC bits needed by FreeRTOS.
 * Ref: STM32F411 RM0383.
 */

#ifndef STM32F411_REGS_H
#define STM32F411_REGS_H

#include <stdint.h>

#define PERIPH_BASE          0x40000000UL
#define APB1PERIPH_BASE      (PERIPH_BASE + 0x00000000UL)
#define AHB1PERIPH_BASE      (PERIPH_BASE + 0x00020000UL)

/* ---- RCC ---- */
#define RCC_BASE             (AHB1PERIPH_BASE + 0x3800UL)
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    uint32_t          RESERVED0[2];
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    uint32_t          RESERVED1[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    uint32_t          RESERVED2[2];
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
} RCC_TypeDef;
#define RCC ((RCC_TypeDef *)RCC_BASE)

#define RCC_AHB1ENR_GPIOAEN   (1UL << 0)
#define RCC_AHB1ENR_GPIOCEN   (1UL << 2)
#define RCC_APB1ENR_USART2EN  (1UL << 17)

/* ---- GPIOA ---- */
#define GPIOA_BASE           (AHB1PERIPH_BASE + 0x0000UL)
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_TypeDef;
#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)

/* GPIOC for onboard LED (PC13 on most Nucleo-F411RE boards) */
#define GPIOC_BASE           (AHB1PERIPH_BASE + 0x0800UL)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)

/* ---- USART2 ---- */
#define USART2_BASE          (APB1PERIPH_BASE + 0x4400UL)
typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_TypeDef;
#define USART2 ((USART_TypeDef *)USART2_BASE)

#define USART_SR_TC    (1UL << 6)
#define USART_SR_TXE   (1UL << 7)
#define USART_SR_RXNE  (1UL << 5)
#define USART_CR1_UE   (1UL << 13)
#define USART_CR1_TE   (1UL << 3)
#define USART_CR1_RE   (1UL << 2)
#define USART_CR1_TXEIE (1UL << 7)

/* USART2 IRQ number for NVIC — RM0383 Table 38 */
#define USART2_IRQn    38

/* ---- NVIC helpers (Cortex-M4 core, not STM32-specific) ---- */
#define NVIC_BASE        0xE000E100UL
typedef struct {
    volatile uint32_t ISER[8];
    uint32_t          RESERVED0[24];
    volatile uint32_t ICER[8];
    uint32_t          RESERVED1[24];
    volatile uint32_t ISPR[8];
    uint32_t          RESERVED2[24];
    volatile uint32_t ICPR[8];
    uint32_t          RESERVED3[24];
    volatile uint32_t IABR[8];
    uint32_t          RESERVED4[56];
    volatile uint8_t  IP[240];
} NVIC_TypeDef;
#define NVIC ((NVIC_TypeDef *)NVIC_BASE)

/* Priority group register — needed to match what FreeRTOS expects */
#define SCB_AIRCR        (*(volatile uint32_t *)0xE000ED0CUL)
#define SCB_AIRCR_PRIGROUP_4_NONE  0x05FA0300UL  /* 4 bits preempt, 0 sub */

static inline void nvic_enable_irq(uint32_t irqn)
{
    NVIC->ISER[irqn >> 5] = (1UL << (irqn & 0x1FUL));
}

static inline void nvic_set_priority(uint32_t irqn, uint8_t priority)
{
    /* Cortex-M4 implements 4 priority bits in the top nibble of each IP byte */
    NVIC->IP[irqn] = (uint8_t)(priority << 4);
}

#endif /* STM32F411_REGS_H */
