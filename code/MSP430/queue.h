/**
 * @file queue.h
 * @date: Dec 19, 2013
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

#define NUM_QUEUE_HEADS 2
#define MAX_QUEUE_SIZE  528

/**
 * @class Queue
 * Multi-Headed Queue
 * This always enqueues at the end but can
 * dequeue from multiple points
 */
class Queue
{
public:
    /**
     * Constructor
     */
    Queue();

    /**
     * Destructor
     */
    ~Queue();

    /**
     * Enqueue a value
     * @param data value to enqueue
     */
    void Enqueue(uint8_t data);

    /**
     * Dequeue a value from the given queue head
     * @param headIndex the queue head to dequeue from
     * @return dequeued value
     */
    uint8_t Dequeue(uint16_t headIndex);

    /**
     * Return length of queue from head specified
     * @param headIndex the queue head to get the length of
     * @return the length of the queue
     */
    uint16_t GetLength(uint16_t headIndex);

    /**
     * Return whether there is data to dequeue for the given queue head
     * @param headIndex the queue head to check for data
     * @return whether the queue head has data in it
     */
    inline bool IsDataAvailable(uint16_t headIndex)
    {
        return (heads[headIndex] != tail);
    }

    /**
     * Find the next available message (if any) to given destination.
     * Must be called periodically to update the queue heads
     * @param headIndex the queue head to check for a message
     * @param dest destination of message to find
     * @param[out] len the length of the message if it was found, unchanged otherwise
     * @return whether a message was found
     */
    bool FindNextMessage(uint16_t headIndex, uint8_t dest, uint16_t& len);

private:
    uint16_t heads[NUM_QUEUE_HEADS];
    uint16_t tail;
    uint8_t array[MAX_QUEUE_SIZE];
};

#endif /* QUEUE_H_ */
