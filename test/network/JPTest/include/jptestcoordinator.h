#ifndef JPTESTCOORDINATOR_H
#define JPTESTCOORDINATOR_H

#include <QStringList>
#include "jpacket.h"
#include "jptestoptions.h"
#include "jptestfilereader.h"

class JPTestCoordinator
{
public:
    JPTestCoordinator();
    ~JPTestCoordinator();
    bool LoadTest(const JPTestOptions& Options);

protected:
    QStringList* myOutbox;
    QStringList* P2Inbox;
    QStringList* P3Inbox;
    QMap<QString, JPacket>* jpacketLib;
    JPTestFileReader* fileReader;

    // Protected methods
    void LoadTestScript(const JPTestOptions &Options, QFile& JPTestFile);
};

#endif // JPTESTCOORDINATOR_H
