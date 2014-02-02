#ifndef JPTESTOPTIONS_H
#define JPTESTOPTIONS_H

#include <QString>
#include "jpacket.h"

/**
 * @brief The JPTESTMODE enum specifies the test mode for a given JPTest.
 * @enum RUN mode is normal run mode. Packets are sent out sequentially as normal.
 * @enum STEP mode allows the user to step through test, packet by packet.
 * @enum DELAY mode inserts a specified delay in between the sending of each packet
 */
enum JPTESTMODE
{
    RUN,
    STEP,
    DELAY
};

enum JPTESTERROR
{
    NO_ERROR,
    PORT_ERROR,
    JPTESTFILE_ERROR
};

struct JPTestOptions
{
    JPTestOptions();
    JPTestOptions(const JPTestOptions& other);
    ~JPTestOptions();

    QString GetJagIDString();
    QString GetP2IDString();
    QString GetP3IDString();

    JPTESTMODE RunMode;
    QString Filename;
    QString JPacketPath;
    QString PortName;
    int NumLoops;   // NumLoops -1 => don't loop, 0 => loop forever, n>0 => loop n times
    int DelaySecs;
    int JaguarID;
    int P2ID;
    int P3ID;
    bool MSP430ModeOn;

protected:
    QString GetIDString(const int& ID);
};

#endif // JPTESTOPTIONS_H
