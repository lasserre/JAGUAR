#ifndef JPOUTBOX_H
#define JPOUTBOX_H

class JPOutbox
{
public:
    JPOutbox();
    bool MoreToSend() const;
    void SendNextPacket();

protected:
    bool moreDataToSend;
};

#endif // JPOUTBOX_H
