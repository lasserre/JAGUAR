// queue.h

#ifndef QUEUE_H_
#define QUEUE_H_

#include "defines.h"
#include <stdint.h>

class Queue
{
public:
  Queue();
  void Enqueue(uint8_t b);
  uint8_t Dequeue();
  uint Length();
  uint8_t View(uint offset);

  static const uint BUFF_LEN = MAX_MESSAGE_LEN * 2;

private:
  uint8_t buff[BUFF_LEN];
  uint head;
  uint tail;
};

#endif // QUEUE_H_
