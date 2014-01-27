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
 * @brief JPacket::JPacket
 * @param Payload
 */
JPacket::JPacket(const QByteArray &Payload) : payload(new QByteArray(Payload))
{
    qDebug() << __FUNCTION__ << ":" << __LINE__;
    this->SetFieldsFromPayload();
}

/**
 * @brief JPacket::JPacket
 * @param JPFilename must be the *full* path to the file, not just the *.jp filename
 */
JPacket::JPacket(const QString &JPFilename) : payload(new QByteArray())
  , DST(0)
  , SRC(0)
{
    qDebug() << __FUNCTION__ << ":" << __LINE__;
    if (this->LoadFromFile(JPFilename))
        this->SetFieldsFromPayload();
    else qDebug() << "ERROR: unable to construct JPacket from " << JPFilename;
}

JPacket::JPacket(const JPacket &other) : payload(new QByteArray(*other.payload))
  , DST(other.DST)
  , SRC(other.SRC)
{
    qDebug() << __FUNCTION__ << ":" << __LINE__ << ": copy constructor called!";
}

/**
 * @brief JPacket destructor
 */
JPacket::~JPacket()
{
    qDebug() << __FUNCTION__ << ":" << __LINE__;
    delete this->payload;
}

void JPacket::SetPayload(const QByteArray &Payload)
{
    *(this->payload) = Payload;
    return;
}

/**
 * @brief JPacket::LoadFromFile loads a JAGUAR packet from a jpacket file (.jp)
 * @param JPFilename is the name of the file with no extension. LoadFromFile will add
 * the .jp extension automatically.
 */
bool JPacket::LoadFromFile(const QString &JPFilename)
{
    QString jpacketFile = JPFilename;
    jpacketFile += ".jp";

    QFile JPFile(jpacketFile);

    if (JPFile.open(QIODevice::ReadOnly))
    {
        SetPayload(JPFile.readAll());
        JPFile.close();
        return true;
    }
    else
    {
        qDebug() << "Unable to open jpacket file (.jp)" + jpacketFile;
        return false;
    }
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
