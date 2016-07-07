#ifndef FINPROGRAMME_H
#define FINPROGRAMME_H

#include "ligne.h"

class FinProgramme :public Ligne
{
public:
    FinProgramme();
    virtual QString gcode_ligne();

};

#endif // FINPROGRAMME_H
