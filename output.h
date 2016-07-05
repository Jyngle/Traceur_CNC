#ifndef OUTPUT_H
#define OUTPUT_H


#include "ligne.h"

class output: public Ligne
{
public:
    output(QString relais, QString val);
    virtual QString gcode_ligne();
private:
    QString _relais,  _val;
};

#endif // OUTPUT_H
