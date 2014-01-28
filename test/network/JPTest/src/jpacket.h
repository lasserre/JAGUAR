#ifndef JPACKET_H
#define JPACKET_H

/**
 * \details
 * JPacket Structure
 * ---------------------------------------------------------------------
 * |[STX][DST][STX][LEN][SEQ][SRC][CMP][MSG][--- PAYLOAD ---][CKA][CKB]|
 * ---------------------------------------------------------------------
 *
 * Legend
 * -------------------------
 * STX - Start byte
 * DST - Destination ID
 * LEN - Length of PAYLOAD.
 *         MAVLEN = LEN + 8
 *         JAGLEN = LEN + 10
 * SEQ - Packet sequence # on a per-component basis
 * SRC - Source ID
 * CMP - Component ID
 * MSG - Message ID
 * PAYLOAD - MAVLINK Payload
 * CKA/CKB - Checksum bytes
 * --------------------------
 */

#include <QByteArray>
#include <QString>
#include <QFile>
#include <QDebug>

/**
 * @brief JAGPACKET defines important constants at the JAGUAR protocol level
 *
 * testing testing
 */
namespace JAGPACKET
{
    // Jaguar header bytes
    const int STXBYTE = 0;
    const int DSTBYTE = 1;

    // Jaguar endpoint IDs
    const int BROADCAST = 0;
    const int MS = 1;
    const int QC = 2;
    const int GCS = 255;

    // Jaguar start byte contents
    const int STXVALUE = 254;
}

namespace MAVPACKET
{
    // Use this to offset into mavlink packet
    const int JHDR_OFFSET = 2;      // Jaguar header only
    const int MVHDR_OFFSET = 6;     // Mavelink header only
    const int tail_CS_OFFSET = 2;   // Checksum offset from tail
    // MAVLINK format
    const int STXBYTE = 0;
    const int LENBYTE = 1;
    const int SEQBYTE = 2;
    const int SRCBYTE = 3;
    const int CMPBYTE = 4;
    const int MSGBYTE = 5;
    const int PAYLOADSTART = 6;

    // MAVLINK start byte contents
    const int STXVALUE = 254;
}

class JPacket
{
public:
    JPacket();
    JPacket(const QByteArray& Payload);
    JPacket(const QString& JPFilename);
    JPacket(const JPacket& other);
    ~JPacket();
    void SetPayload(const QByteArray& Payload);
    bool LoadFromFile(const QString& JPFilename);
    QByteArray GetPayload() const;
    int GetSrc() const;
    int GetDst() const;

protected:
    QByteArray* payload;
    int DST;
    int SRC;
    //int LEN; do we need?

    void SetFieldsFromPayload();    // Call to set DST, SRC from payload if payload is already in memory
};

#endif // JPACKET_H
