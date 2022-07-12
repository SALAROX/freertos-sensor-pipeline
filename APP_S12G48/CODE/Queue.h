/*
 * Queue.h
 *
 *  Created on: Sep 10, 2020
 *      Author: CTLWKS037
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#define QSIZE   256//128

uint8_t isFull (void);

uint8_t isEmpty (void);

uint8_t push (uint8_t item);

uint8_t pop (uint8_t *loc);

uint8_t get_size (void);

#endif /* QUEUE_H_ */
