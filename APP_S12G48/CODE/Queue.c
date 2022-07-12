/*
 * Queue.c
 *
 *  Created on: Sep 10, 2020
 *      Author: CTLWKS037
 */

#include "PE_Types.h"
#include "Queue.h"

#define MASK ((QSIZE)-1)

uint8_t q[QSIZE];
uint8_t head, tail;

static uint8_t inc (uint8_t x)
{
  return (x + 1) & MASK;
}

uint8_t isFull (void)
{
  return inc (head) == tail;
}

uint8_t isEmpty (void)
{
  return head == tail;
}

uint8_t push (uint8_t item)
{
  if (isFull ())
    return 0;
  q[head] = item;
  head = inc (head);
  return 1;
}

uint8_t pop (uint8_t *loc)
{
  if (isEmpty())
    return 0;
  *loc = q[tail];
  tail = inc (tail);
  return 1;
}

uint8_t get_size (void)
{
  int8_t s = head - tail;
  if (s < 0)
    s += (QSIZE);
  return s;
}
