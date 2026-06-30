/*
 * uart.c — interrupt-driven TX with a ring buffer, blocking RX (not used
 * much in this project but left in for completeness).
 *
 * TX ring buffer means the reporter task can hand off a string and get back
 * to work immediately; the ISR drains it in the background. This matters
 * once you have multiple tasks: spinning in a tight poll loop for TX in one
 * task starves everything at the same or lower priority.
 */

#include "uart.h"
#include "stm32f411_regs.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define TX_BUF_SIZE  256
#define SYSCLK_HZ    16000000UL

static volatile uint8_t  tx_buf[TX_BUF_SIZE];
static volatile uint16_t tx_head = 0;
static volatile uint16_t tx_tail = 0;

/* Protects tx_head/tx_tail in the non-ISR path */
static SemaphoreHandle_t tx_mutex;

void uart_init(uint32_t baud)
{
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR  |= RCC_APB1ENR_USART2EN;

    GPIOA->MODER  &= ~((3UL << (2*2)) | (3UL << (3*2)));
    GPIOA->MODER  |=  ((2UL << (2*2)) | (2UL << (3*2)));
    GPIOA->AFR[0] &= ~((0xFUL << (4*2)) | (0xFUL << (4*3)));
    GPIOA->AFR[0] |=  ((7UL  << (4*2)) | (7UL  << (4*3)));  /* AF7 = USART2 */

    USART2->BRR = SYSCLK_HZ / baud;
    USART2->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    /* ISR at priority 6 — numerically above configMAX_SYSCALL_INTERRUPT_PRIORITY
     * (which is 5), so fromISR API calls are safe here */
    nvic_set_priority(USART2_IRQn, 6);
    nvic_enable_irq(USART2_IRQn);

    tx_mutex = xSemaphoreCreateMutex();
    configASSERT(tx_mutex != NULL);
}

void uart_write(const char *s, size_t len)
{
    if (xSemaphoreTake(tx_mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
        return;  /* drop rather than block indefinitely */
    }

    for (size_t i = 0; i < len; i++) {
        uint16_t next = (tx_head + 1) % TX_BUF_SIZE;
        if (next == tx_tail) {
            /* Buffer full — truncate. In practice this means the reporter
             * is sending faster than the line can drain, which shouldn't
             * happen at 115200 with a 500 ms report interval. */
            break;
        }
        tx_buf[tx_head] = (uint8_t)s[i];
        tx_head = next;
    }

    /* Kick the TX ISR if it's not already running */
    USART2->CR1 |= USART_CR1_TXEIE;

    xSemaphoreGive(tx_mutex);
}

/* USART2 IRQ handler — drains the ring buffer one byte at a time */
void USART2_IRQHandler(void)
{
    if (USART2->SR & USART_SR_TXE) {
        if (tx_tail != tx_head) {
            USART2->DR = tx_buf[tx_tail];
            tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
        } else {
            /* Nothing left to send — disable TXE interrupt until next write */
            USART2->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}

uint8_t uart_read_byte(void)
{
    while (!(USART2->SR & USART_SR_RXNE)) {}
    return (uint8_t)USART2->DR;
}
