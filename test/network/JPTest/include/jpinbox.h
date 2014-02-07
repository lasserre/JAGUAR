#ifndef JPINBOX_H
#define JPINBOX_H

#include <QDebug>

class JPInbox
{
public:
    JPInbox();
    void CheckMail();
    bool MoreToReceive() const;

protected:
    bool moreDataToReceive;
};

#endif // JPINBOX_H
