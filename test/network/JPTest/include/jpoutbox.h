#ifndef JPOUTBOX_H
#define JPOUTBOX_H

#include <QDebug>
#include <QByteArray>

class JPOutbox : public QObject
{
    Q_OBJECT

public:
    explicit JPOutbox(QObject* parent = 0);
    bool MoreToSend() const;
    QByteArray GetSendNextPacket();
    void SetNextPacket(const QByteArray& NextPacket);

protected:
    QByteArray currentPacketData;
};

#endif // JPOUTBOX_H
