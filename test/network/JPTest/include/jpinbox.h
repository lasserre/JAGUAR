#ifndef JPINBOX_H
#define JPINBOX_H

#include <QDebug>
#include "jpacket.h"
#include "jpacketchecker.h"

class JPInbox : public QObject
{
    Q_OBJECT

public:
    explicit JPInbox(QObject* parent = 0);
    ~JPInbox();
    JPacketDiffResults CheckMail();
    bool WaitingOnP2Data() const;
    bool WaitingOnP3Data() const;
    void AddToInbox(const QByteArray& Bytes);
    void SetNextP2ExpectedPacket(const JPacket& NextP2Packet);
    void SetNextP3ExpectedPacket(const JPacket& NextP3Packet);
    void SetP2ID(const int& ID);
    void SetP3ID(const int& ID);
    void SetMSP430Mode(const bool& Enabled = true);
    void Clear();

protected:
    JPacketChecker* packetInspector;
    QByteArray byteInbox;
};

#endif // JPINBOX_H
