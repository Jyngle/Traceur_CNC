#ifndef PAUSE_H
#define PAUSE_H

#include "ligne.h"

class pause: public Ligne
{
public:
    pause(QString delay);
    virtual QString gcode_ligne();
private:
    QString _delay;
};

#endif // PAUSE_H
