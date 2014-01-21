#include "jpacket.h"

/**
 * @brief JPacket::JPacket
 */
JPacket::JPacket() : payload(new QByteArray())
  , DST(0)
  , SRC(0)
{
}

/**
 * @brief JPacket destructor
 */
JPacket::~JPacket()
{
    delete this->payload;
}

/**
 * @brief JPacket::JPacket
 * @param Payload
 */
JPacket::JPacket(const QByteArray &Payload) : payload(new QByteArray(Payload))
{
    this->SetFieldsFromPayload();
}

JPacket::JPacket(const QString &JPFilename) : payload(new QByteArray())
  , DST(0)
  , SRC(0)
{
    this->LoadFromFile(JPFilename);
}

void JPacket::SetPayload(const QByteArray &Payload)
{
    *(this->payload) = Payload;
    return;
}

void JPacket::LoadFromFile(const QString &JPFilename)
{
    QFile JPFile(JPFilename);
    if (JPFile.open(QIODevice::ReadOnly))
    {
        *(this->payload) = JPFile.readAll();
    }
    else
    {
        qDebug() << "Unable to open file " + JPFilename;
    }
    return;
}

void JPacket::SetFieldsFromPayload()
{
    this->DST = payload->at(JAGPACKET::DSTBYTE);
    this->SRC = payload->at(MAVPACKET::JHDR_OFFSET + MAVPACKET::SRCBYTE);
    return;
}

QByteArray JPacket::GetPayload() const
{
    return *(this->payload);
}

int JPacket::GetDst() const
{
    return this->DST;
}

int JPacket::GetSrc() const
{
    return this->SRC;
}
