#ifndef G00_H
#define G00_H

#include "deplacement.h"

class G00 : public Deplacement{

public :
    G00(float _X, float _Y, float _Z, float _F);
    virtual QList<float> get_info_abs();
    virtual QList<float> get_info_rel();
    virtual float get_distance();
    virtual QString gcode_ligne();
    virtual float get_time();
};

#endif // G00_H
