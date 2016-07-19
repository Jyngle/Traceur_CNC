#ifndef G03_H
#define G03_H

#include "arquedecercle.h"

class G03 : public ArqueDeCercle{

public :
    G03(float _X, float _Y, float _Z, float _I, float _J, float _F);
    virtual QString gcode_ligne();
};

#endif // G03_H
