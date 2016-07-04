#ifndef LIGNE_H
#define LIGNE_H

#include <QString>

class Ligne{

public:
    virtual QString gcode_ligne() = 0;

};


#endif // LIGNE_H
