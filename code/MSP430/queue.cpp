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

uint8_t Queue::View(uint16_t headIndex, uint16_t dataIndex) const
{
    uint16_t index = heads[headIndex] + dataIndex;
    if (index >= MAX_QUEUE_SIZE)
    {
        index -= MAX_QUEUE_SIZE;
    }
    return array[index];
}

uint16_t Queue::GetLength(uint16_t headIndex) const
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
    AlignWithMessageHeader(headIndex);

    while (GetLength(headIndex) >= MIN_LEN_FOR_ROUTING && IsAtMessageHeader(headIndex) && !foundMsg && !passedTail)
    {
        // check if the message destination ID matches the one we're looking for
        destId = View(msgIndex, JAGUAR_DEST_OFFSET);
        if (destId == dest || destId == JAGUAR_DEST_ALL)
        {
            foundMsg = true;
        }

        // find the start of the next message
        if (!foundMsg)
        {
            totalLen = JAGUAR_HEADER_LEN + MAVLINK_HEADER_SIZE + MAVLINK_TAIL_SIZE + View(msgIndex, JAGUAR_LEN_OFFSET);
            if (totalLen <= GetLength(headIndex))
            {
                msgIndex += totalLen;
                if (msgIndex >= MAX_QUEUE_SIZE)
                {
                    msgIndex -= MAX_QUEUE_SIZE;
                }

                heads[headIndex] = msgIndex;

                // check if we are aligned at the start of a message
                AlignWithMessageHeader(headIndex);

            }
            else
            {
                passedTail = true;
            }
        }
    }

    if (foundMsg)
    {
        // pass back the message length
        len = MAVLINK_HEADER_SIZE + MAVLINK_TAIL_SIZE + View(heads[headIndex], JAGUAR_LEN_OFFSET);

        // skip over the JAGUAR destination header
        heads[headIndex] += JAGUAR_HEADER_LEN;
    }

    return foundMsg;
}

bool Queue::IsAtMessageHeader(uint16_t headIndex) const
{
    return (array[headIndex] == MAVLINK_STX && View(headIndex, JAGUAR_HEADER_LEN) == MAVLINK_STX);
}

void Queue::AlignWithMessageHeader(uint16_t headIndex)
{
    while (GetLength(headIndex) >= MIN_LEN_FOR_ROUTING && !IsAtMessageHeader(headIndex))
    {
        // If we get here, one of the enqueued messages is corrupt.
        // All we can do is search for a start signature and hope
        // it's the beginning of the next message
        if (heads[headIndex] == MAX_QUEUE_SIZE - 1)
        {
            heads[headIndex] = 0;
        }
        else
        {
            ++(heads[headIndex]);
        }
    }
}
