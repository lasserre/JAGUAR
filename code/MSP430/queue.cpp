/**
 * @file queue.cpp
 * @date Dec 19, 2013
 */

#include "queue.h"
#include "defines.h"

Queue::Queue()
{
    for (uint16_t i = 0; i < NUM_QUEUE_HEADS; ++i)
    {
        heads[i] = 0;
    }
    tail = 0;
}

Queue::~Queue()
{
}

void Queue::Enqueue(uint8_t data)
{
    array[tail] = data;
    if (tail == MAX_QUEUE_SIZE - 1)
    {
        tail = 0;
    }
    else
    {
        ++tail;
    }
}

uint8_t Queue::Dequeue(uint16_t headIndex)
{
    uint8_t data = array[heads[headIndex]];
    if (heads[headIndex] == MAX_QUEUE_SIZE - 1)
    {
        heads[headIndex] = 0;
    }
    else
    {
        ++(heads[headIndex]);
    }

    return data;
}

uint16_t Queue::GetLength(uint16_t headIndex)
{
    uint16_t index = heads[headIndex];
    return (index <= tail) ? (tail - index) : (tail + (MAX_QUEUE_SIZE - index));
}

bool Queue::FindNextMessage(uint16_t headIndex, uint8_t dest, uint16_t& len)
{
    //TODO: does this function need interrupt guards?
    bool foundMsg = false;
    bool passedTail = false;
    uint16_t msgIndex;
    uint16_t totalLen;
    uint8_t destId;

    msgIndex = heads[headIndex];
    // check if we are aligned at the start of a message
    while (array[msgIndex] != MAVLINK_STX && IsDataAvailable(msgIndex))
    {
        // If we get here, one of the enqueued messages is corrupt.
        // All we can do is search for a start signature and hope
        // it's the beginning of the next message
        ++msgIndex;
    }
    while (GetLength(headIndex) >= MAVLINK_HEADER_SIZE && !foundMsg && !passedTail)
    {
        // check if the message dest ID matches the one we're looking for
        destId = array[msgIndex + MAVLINK_DEST_OFFSET];
        if (destId == dest || destId == MAVLINK_DEST_ALL)
        {
            foundMsg = true;
        }

        // find the start of the next message
        if (!foundMsg)
        {
            totalLen = MAVLINK_HEADER_SIZE + MAVLINK_TAIL_SIZE + array[msgIndex + MAVLINK_LEN_OFFSET];
            if (totalLen <= GetLength(headIndex))
            {
                msgIndex += totalLen;
                if (msgIndex >= MAX_QUEUE_SIZE)
                {
                    msgIndex -= MAX_QUEUE_SIZE;
                }

                // check if we are aligned at the start of a message
                while (array[msgIndex] != MAVLINK_STX && IsDataAvailable(msgIndex))
                {
                    // If we get here, one of the enqueued messages is corrupt.
                    // All we can do is search for a start signature and hope
                    // it's the beginning of the next message
                    ++msgIndex;
                }
                heads[headIndex] = msgIndex;
            }
            else
            {
                passedTail = true;
            }
        }
    }

    if (foundMsg)
    {
        len = MAVLINK_HEADER_SIZE + MAVLINK_TAIL_SIZE + array[msgIndex + MAVLINK_LEN_OFFSET];
    }

    return foundMsg;
}
