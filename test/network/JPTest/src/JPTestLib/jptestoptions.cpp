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
QString JPTestOptions::GetJagIDString() const
{
    return GetIDString(JaguarID);
}

QString JPTestOptions::GetP2IDString() const
{
    return GetIDString(P2ID);
}

QString JPTestOptions::GetP3IDString() const
{
    return GetIDString(P3ID);
}

QString JPTestOptions::GetIDString(const int& ID) const
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

QString JPTestOptions::GetRunModeString() const
{
    switch(RunMode)
    {
    case RUN:
        return "RUN";
        break;
    case STEP:
        return "STEP";
        break;
    case DELAY:
        return "DELAY";
        break;
    default:
        return "Error: Unknown Run Mode";
    }
}
