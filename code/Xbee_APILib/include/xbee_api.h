#ifndef XBEE_API_H
#define XBEE_API_H

#ifndef NULL
#define NULL 0
#endif


/* ---------- Frame Types ---------- */
#define FT_TRANSMIT_REQUEST 0x10

namespace XbeeAPI
{
    namespace Frame
    {
        enum Type
        {
            TransmitRequest
        };
    }

    /**
     * @brief The XbeeFrame class ...
     * THE DEFAULT CONSTRUCTOR DOES NOT ALLOCATE MEMORY FOR PAYLOADSTART!!!!!!!!
     */
    class XbeeFrame
    {
        XbeeFrame();
        XbeeFrame(const XbeeFrame& Other);
        ~XbeeFrame();

        /**
         * @brief GetPayloadStart
         * @param PayloadBegin
         * @return TRUE if payload memory is allocated, false otherwise (BEWARE OF NULL!!)
         */
        bool GetPayloadStart(char* PayloadBegin) const;
        int GetPayloadLength() const;

        Frame::Type GetFrameType() const;
        int GetFrameLength() const;

        /**
         * @brief GetFrameStart
         * @param FrameStart
         * @param FrameLength
         * @return TRUE if frame memory is allocated, false otherwise (BEWARE NULL PTRS!!!)
         */
        bool GetFrameStart(char* FrameStart, int FrameLength) const;

    protected:
        Frame::Type frameType;
        int frameLength;
        char* frameStart;
    };

    int CalculateChecksum(int PacketSize, char* PacketStart);
}

#endif // XBEE_API_H

