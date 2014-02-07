#include "jpoutbox.h"

JPOutbox::JPOutbox() : moreDataToSend(false)
{

}

bool JPOutbox::MoreToSend() const
{
    return moreDataToSend;
}

void JPOutbox::SendNextPacket()
{
    return;
}
