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

const bool AIRSHIP_FIND_WHOLE_MSG = true;

enum ComId
{
    GCS,
    AIRSHIP,
    ROTORCRAFT,
    NUM_COM_IDS
};

/**
 * @class ComManager
 * This is a singleton class that manages the communication between
 * the GCS, Airship, and Rotorcraft
 */
class ComManager
{
public:
    /**
     * Get the ComManager instance
     * @return a pointer to the ComManager instance
     */
    static ComManager* GetInstance();

    /**
     * The main loop for processing messages
     */
    void Mainloop();

private:
    static Queue gcsQ;                     ///< queue for incoming messages from GCS
    static Queue airshipQ;                 ///< queue for incoming messages from Airship
    static Queue rotorcraftQ;              ///< queue for incoming messages from Rotorcraft
    static uint16_t txTimers[NUM_COM_IDS]; ///< timers to keep track of when outgoing messages have timed out
    static bool updateTimers;              ///< value to determine when to update timers
    static ComId gcsFromId;                ///< ID of the link we're transmitting from to the GCS
    static ComId airshipFromId;            ///< ID of the link we're transmitting from to the Airship
    static ComId rotorcraftFromId;         ///< ID of the link we're transmitting from to the Rotorcraft
    static uint16_t gcsTxLen;              ///< length of the message currently being transmitted to the GCS
    static uint16_t airshipTxLen;          ///< length of the message currently being transmitted to the Airship
    static uint16_t rotorcraftTxLen;       ///< length of the message currently being transmitted to the Rotorcraft
    static uint16_t gcsTxCount;            ///< counter for message currently being transmitted to the GCS
    static uint16_t airshipTxCount;        ///< counter for message currently being transmitted to the Airship
    static uint16_t rotorcraftTxCount;     ///< counter for message currently being transmitted to the Rotorcraft
    uint8_t routeTable[NUM_COM_IDS];       ///< table containing destination IDs

    /**
     * Constructor
     */
    ComManager();

    /**
     * Destructor
     */
    ~ComManager();

    /**
     * Copy constructor
     */
    ComManager(const ComManager& manager);

    /**
     * Overloaded assignment operator
     */
    ComManager& operator=(const ComManager& manager);

    /**
     * Interrupt service routine for UART A0
     */
    static __interrupt void USCI_A0_ISR();

    /**
     * Interrupt service routine for UART A1
     */
    static __interrupt void USCI_A1_ISR();

    /**
     * Interrupt service routine for I2C
     */
    static __interrupt void USCI_B1_ISR();

    /**
     * Interrupt service routine for Timer A to
     * increment message timeout timers
     */
    static __interrupt void TIMER0_A0_ISR();

    /**
     * Initialize the UART and I2C interfaces
     */
    void InitUsci();

    /**
     * Initialize the timer module
     */
    void InitTimer();

    /**
     * Updates the timers
     */
    inline void UpdateTimers()
    {
        static uint16_t timerIdx = 0; // index of timer to update on current function call

        // update a different timer on each call, this saves time
        // rather than updating every timer on every call
        if (timerIdx == NUM_COM_IDS - 1)
        {
            timerIdx = 0;
        }
        else
        {
            ++timerIdx;
        }

        uint16_t &timer = txTimers[timerIdx];
        if (timer > 0)
        {
            --timer;
            if (timer == 0)
            {
                switch (timerIdx)
                {
                case GCS:
                    switch (gcsFromId)
                    {
                    case AIRSHIP:
                        airshipQ.MessageTimeout(gcsTxLen - gcsTxCount);
                        break;
                    case ROTORCRAFT:
                        rotorcraftQ.MessageTimeout(gcsTxLen - gcsTxCount);
                        break;
                    default:
                        // we should never get here
                        break;
                    }
                    break;
                case AIRSHIP:
                    //TODO: evaluate if this is needed as we may wait for the entire message to arrive if it is going to the airship
                    switch (airshipFromId)
                    {
                    case GCS:
                        gcsQ.MessageTimeout(airshipTxLen - airshipTxCount);
                        break;
                    case ROTORCRAFT:
                        rotorcraftQ.MessageTimeout(airshipTxLen - airshipTxCount);
                        break;
                    default:
                        // we should never get here
                        break;
                    }
                    break;
                case ROTORCRAFT:
                    switch (rotorcraftFromId)
                    {
                    case GCS:
                        gcsQ.MessageTimeout(rotorcraftTxLen - rotorcraftTxCount);
                        break;
                    case AIRSHIP:
                        airshipQ.MessageTimeout(rotorcraftTxLen - rotorcraftTxCount);
                        break;
                    default:
                        // we should never get here
                        break;
                    }
                    break;
                default:
                    // we should never get here
                    break;
                }
            }
        }
    }
};

#endif /* COMMANAGER_H_ */
