/*
 * queue.h
 *
 *  Created on: 10.09.2019
 *      Author: Andreas
 */

#ifndef HYBRID_CONV_LIB_QUEUE_H_
#define HYBRID_CONV_LIB_QUEUE_H_

/** Inits the queue */
#define queue_Init(_queue) _queue.head = 0; _queue.tail = 0
/** checks if the queue is not empty */
#define queue_NotEmpty(_queue) (_queue.head != _queue.tail)
/** Gets the first element of the queue */
#define queue_Get(_queue) _queue.queue[_queue.tail]
/** Sets the last element of the queue */
#define queue_Set(_queue) _queue.queue[_queue.head]
/** Checks if the queue is full */
#define queue_Full(_queue, size) (((_queue.head + 1) % size) == _queue.tail)
/** pushes one element to the queue and increments the counter */
#define queue_Push(_queue, size) _queue.head = (_queue.head + 1) % size
/** Subtracts one element from the queue */
#define queue_Pop(_queue, size) _queue.queue[_queue.tail = (_queue.tail + 1) % size]


#endif /* HYBRID_CONV_LIB_QUEUE_H_ */
