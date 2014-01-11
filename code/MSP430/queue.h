/**
 * @file queue.h
 * @date: Dec 19, 2013
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

/**
 * @class Queue
 * Multi-Headed Queue
 * This always enqueues at the end but can
 * dequeue from multiple points
 */
class Queue
{
public:
    static const uint16_t NUM_QUEUE_HEADS = 2;  ///< the number of heads in the queue
    static const uint16_t MAX_QUEUE_SIZE = 530; ///< the maximum size of the queue

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
    uint16_t GetLength(uint16_t headIndex) const;

    /**
     * Return whether there is data to dequeue for the given queue head
     * @param headIndex the queue head to check for data
     * @return whether the queue head has data in it
     */
    inline bool IsDataAvailable(uint16_t headIndex) const
    {
        return (heads[headIndex] != tail);
    }

    /**
     * Find the next available message (if any) to given destination.
     * Must be called periodically to update the queue heads. This
     * should NOT be called while in the middle of dequeueing a
     * message
     * @param headIndex the queue head to check for a message
     * @param dest destination of message to find
     * @param[out] len the length of the message if it was found, unchanged otherwise
     * @return whether a message was found
     */
    bool FindNextMessage(uint16_t headIndex, uint8_t dest, uint16_t& len);

private:
    uint16_t heads[NUM_QUEUE_HEADS]; ///< the queue head indices
    uint16_t tail;                   ///< the queue tail index
    uint8_t array[MAX_QUEUE_SIZE];   ///< the queue contents

    /**
     * Returns the value in the queue at index base + offset correcting
     * for wrap around if necessary. This does not modify the queue
     * @param base the base queue index
     * @param offset the offset of the data from base
     * @return the data at index base + offset
     */
    uint8_t ViewOffset(uint16_t base, uint16_t offset) const;

    /**
     * Checks if the queue head is aligned at the beginning
     * of a message. Before calling this method, it should
     * be verified that the queue length is at least
     * MIN_LEN_FOR_ROUTING
     * @param headIndex the queue head to check for alignment
     * @return whether the queue head is aligned to the start of a message
     */
    bool IsAtMessageHeader(uint16_t headIndex) const;

    /**
     * Attempts to align queue head with message header. It may
     * not be successful, so IsAtMessageHeader() should be used
     * to check whether it is actually aligned
     * @param headIndex the queue head to attempt to align
     */
    void AlignWithMessageHeader(uint16_t headIndex);
};

#endif /* QUEUE_H_ */
