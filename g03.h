#ifndef G03_H
#define G03_H

#include "arquedecercle.h"

class G03 : public ArqueDeCercle{

public :
    G03(long double _X, long double _Y, long double _Z, long double _I, long double _J, long double _F);
    virtual QString gcode_ligne();
};

#endif // G03_H
