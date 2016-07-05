#ifndef INPUT_H
#define INPUT_H

#include "ligne.h"

class input: public Ligne
{
public:
    input(QString pin, QString val);
    virtual QString gcode_ligne();
private:
    QString _pin,  _val;
};

#endif // INPUT_H
