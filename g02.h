#ifndef G02_H
#define G02_H

#include "arquedecercle.h"

class G02 : public ArqueDeCercle{

public :
    G02(float _X, float _Y, float _Z, float _I, float _J, float _F);
    virtual QString gcode_ligne();
};

#endif // G02_H
