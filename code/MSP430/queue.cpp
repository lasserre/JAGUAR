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

bool Queue::FindNextMessage(uint16_t headIndex, uint8_t dest, uint16_t& len)
{
    bool foundMsg = false;
    bool passedTail = false;
    uint16_t msgIndex;
    uint16_t totalLen;
    uint8_t destId;
    uint16_t &head = heads[headIndex];

    msgIndex = head;

    // check if we are aligned at the start of a message
    AlignWithMessageHeader(headIndex);

    while (!foundMsg && !passedTail && GetLength(headIndex) >= MIN_LEN_FOR_ROUTING && IsAtMessageHeader(headIndex))
    {
        // check if the message destination ID matches the one we're looking for
        destId = ViewOffset(msgIndex, JAGUAR_DEST_OFFSET);
        if (destId == dest || destId == JAGUAR_DEST_ALL)
        {
            foundMsg = true;

            // pass back the message length
            len = MAVLINK_HEADER_SIZE + MAVLINK_TAIL_SIZE + ViewOffset(head, JAGUAR_LEN_OFFSET);

            // skip over the JAGUAR destination header
            head += JAGUAR_HEADER_LEN;
            if (head >= MAX_QUEUE_SIZE)
            {
                head -= MAX_QUEUE_SIZE;
            }
        }
        else // find the start of the next message
        {
            totalLen = JAGUAR_HEADER_LEN + MAVLINK_HEADER_SIZE + MAVLINK_TAIL_SIZE + ViewOffset(msgIndex, JAGUAR_LEN_OFFSET);
            if (totalLen <= GetLength(headIndex))
            {
                msgIndex += totalLen;
                if (msgIndex >= MAX_QUEUE_SIZE)
                {
                    msgIndex -= MAX_QUEUE_SIZE;
                }

                head = msgIndex;

                // check if we are aligned at the start of a message
                AlignWithMessageHeader(headIndex);

            }
            else
            {
                passedTail = true;
            }
        }
    }

    return foundMsg;
}

bool Queue::IsAtMessageHeader(uint16_t headIndex) const
{
    const uint16_t &head = heads[headIndex];
    return (array[head] == MAVLINK_STX && ViewOffset(head, JAGUAR_HEADER_LEN) == MAVLINK_STX);
}

void Queue::AlignWithMessageHeader(uint16_t headIndex)
{
    while (GetLength(headIndex) >= MIN_LEN_FOR_ROUTING && !IsAtMessageHeader(headIndex))
    {
        // If we get here, one of the enqueued messages is corrupt.
        // All we can do is search for a start signature and hope
        // it's the beginning of the next message
        uint16_t &head = heads[headIndex];
        if (head == MAX_QUEUE_SIZE - 1)
        {
            head = 0;
        }
        else
        {
            ++head;
        }
    }
}
