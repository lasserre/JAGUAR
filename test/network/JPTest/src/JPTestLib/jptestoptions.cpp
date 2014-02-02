// jptestoptions.cpp

#include "jptestoptions.h"

JPTestOptions::JPTestOptions() : RunMode(RUN)
  , Filename("UnsetFilename")
  , JPacketPath("UnsetJPacketPath")
  , PortName("UnsetPortName")
  , NumLoops(-1)
  , DelaySecs(-1)
  , JaguarID(JAGPACKET::GCS)
  , P2ID(JAGPACKET::MS)
  , P3ID(JAGPACKET::QC)
  , MSP430ModeOn(true)
{
}

JPTestOptions::JPTestOptions(const JPTestOptions& other) : RunMode(other.RunMode)
  , Filename(other.Filename)
  , JPacketPath(other.JPacketPath)
  , PortName(other.PortName)
  , NumLoops(other.NumLoops)
  , DelaySecs(other.DelaySecs)
  , JaguarID(other.JaguarID)
  , P2ID(other.P2ID)
  , P3ID(other.P3ID)
  , MSP430ModeOn(other.MSP430ModeOn)
{
}

JPTestOptions::~JPTestOptions()
{
}

/**
 * @brief GetJagIDString
 * @return the string of MY Jaguar ID
 */
QString JPTestOptions::GetJagIDString()
{
    return GetIDString(JaguarID);
}

QString JPTestOptions::GetP2IDString()
{
    return GetIDString(P2ID);
}

QString JPTestOptions::GetP3IDString()
{
    return GetIDString(P3ID);
}

QString JPTestOptions::GetIDString(const int& ID)
{
    switch(ID)
    {
    case JAGPACKET::GCS:
        return "GCS";
    case JAGPACKET::MS:
        return "MS";
    case JAGPACKET::QC:
        return "QC";
    case JAGPACKET::BROADCAST:
        return "Broadcast";
    default:
        return "JPTestOptions: unrecognized JAGUAR ID!";
    }
}
