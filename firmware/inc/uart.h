#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stddef.h>

void    uart_init(uint32_t baud);
void    uart_write(const char *s, size_t len);
uint8_t uart_read_byte(void);

#endif
