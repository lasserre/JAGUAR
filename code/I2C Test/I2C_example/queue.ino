// queue.cpp

#include "queue.h"

Queue::Queue()
{
  head = 0;
  tail = 0;
}

void Queue::Enqueue(uint8_t b)
{
  buff[tail] = b;
  if (tail == BUFF_LEN - 1)
  {
    tail = 0;
  }
  else
  {
    ++tail;
  }
}

uint8_t Queue::Dequeue()
{
  uint8_t b = buff[head];
  if (head == BUFF_LEN - 1)
  {
    head = 0;
  }
  else
  {
    ++head;
  }
  return b;
}

uint Queue::Length()
{
  return (head <= tail) ? (tail - head) : (tail + (BUFF_LEN - head));
}

uint8_t Queue::View(uint offset)
{
  uint index = head + offset;
  if (index >= BUFF_LEN)
  {
    index -= BUFF_LEN;
  }
  
  return buff[index];
}

void Queue::Clear()
{
  head = tail;
}

