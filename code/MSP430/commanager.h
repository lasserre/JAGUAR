/**
 * @file: commanager.h
 * @date: Dec 31, 2013
 */

#ifndef COMMANAGER_H_
#define COMMANAGER_H_

#include "queue.h"

// indexes for queue heads
#define GCS_LINK_AIRSHIP_HEAD           0
#define GCS_LINK_ROTORCRAFT_HEAD        1
#define AIRSHIP_LINK_GCS_HEAD           0
#define AIRSHIP_LINK_ROTORCRAFT_HEAD    1
#define ROTORCRAFT_LINK_GCS_HEAD        0
#define ROTORCRAFT_LINK_AIRSHIP_HEAD    1

enum ComId
{
    GCS,
    AIRSHIP,
    ROTORCRAFT,
    NUM_COM_IDS
};

/**
 * @class ComManager
 * This class manages the communication between
 * the GCS, Airship, and Rotorcraft
 */
class ComManager
{
public:
    /**
     * Constructor
     */
    ComManager();

    /**
     * Destructor
     */
    ~ComManager();

    /**
     * The main loop for processing messages
     */
    void Mainloop();

private:
    Queue gcsQ;                      ///< queue for incoming messages from GCS
    Queue airshipQ;                  ///< queue for incoming messages from Airship
    Queue rotorcraftQ;               ///< queue for incoming messages from Rotorcraft
    ComId gcsFromId;                 ///< ID of the link we're transmitting from to the GCS
    ComId airshipFromId;             ///< ID of the link we're transmitting from to the Airship
    ComId rotorcraftFromId;          ///< ID of the link we're transmitting from to the Rotorcraft
    uint16_t gcsTxLen;               ///< length of the message currently being transmitted to the GCS
    uint16_t airshipTxLen;           ///< length of the message currently being transmitted to the Airship
    uint16_t rotorcraftTxLen;        ///< length of the message currently being transmitted to the Rotorcraft
    uint16_t gcsTxCount;             ///< counter for message currently being transmitted to the GCS
    uint16_t airshipTxCount;         ///< counter for message currently being transmitted to the Airship
    uint16_t rotorcraftTxCount;      ///< counter for message currently being transmitted to the Rotorcraft
    uint8_t routeTable[NUM_COM_IDS]; ///< table containing destination IDs

    /**
     * Interrupt service routine for UART A0
     */
    static __interrupt void USCI_A0_ISR();

    /**
     * Interrupt service routine for UART A1
     */
    static __interrupt void USCI_A1_ISR();

    //TODO: add interrupt service routine for timer; discard message if havn't received header, send 0's if have

    /**
     * Initialize the timers for signaling when
     * receiving a message has timed out
     */
    void InitTimers();

    /**
     * Initialize the UART and I2C interfaces
     */
    void InitUsci();
};

#endif /* COMMANAGER_H_ */
