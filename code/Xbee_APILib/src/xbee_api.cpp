#include "xbee_api.h"

using namespace XbeeAPI;

int CalculateChecksum(int PacketSize, char *PacketStart)
{
    return 0;
}

XbeeFrame::XbeeFrame() : frameType(Frame::TransmitRequest)
  , frameLength(-1)
  , frameStart(NULL)
{
}

XbeeFrame::XbeeFrame(const XbeeFrame &Other) : frameType(Other.frameType)
  , frameLength(Other.frameLength)
  , frameStart(NULL)
{
    if (Other.frameStart != NULL)
    {
        frameStart = new char[Other.frameLength];

        char* myFrameMemory = this->frameStart;     // Temp ptr to walk through our frame's memory

        for (int i = 0; i < Other.frameLength; i++)
            myFrameMemory[i] = Other.frameStart[i];
    }
}

XbeeFrame::~XbeeFrame()
{
    if (frameStart != NULL)
    {
        for (int i = 0; i < frameLength; i++)
            delete frameStart++;
    }
}

/**
 * @brief XbeeFrame::GetPayloadStart
 *
 * CLS: When adding a new frame type (class derived from XbeeFrame), be sure to add a switch case
 * to this function to handle the new frame type. The reasoning behind this design is to avoid using
 * polymorphic functions (logically the better choice) so we can use this code on both desktop and embedded
 * platforms (e.g. the APMs)
 *
 * @param PayloadBegin
 * @return
 */
bool XbeeFrame::GetPayloadStart(char *PayloadBegin) const
{
    if (this->frameStart == NULL)
        return false;

    switch (this->frameType)
    {
    case Frame::TransmitRequest:
        // Set PayloadBegin to index into the start of the payload...
        break;
    default:
        return false;
    }

    return true;
}
