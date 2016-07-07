#ifndef G02_H
#define G02_H

#include "deplacement.h"

class G02 : public Deplacement{

public :
    G02(float _X, float _Y, float _Z, float _I, float _J, float _F);
    virtual QList<float> get_info_abs();
    virtual QList<float> get_info_rel();
    virtual float get_distance();
    virtual QString gcode_ligne();
    virtual void set_info_abs(float _X, float _Y);
    virtual void set_info_rel(float _X, float _Y);
    virtual float get_Z();

private:
    float I = 0;
    float J = 0;
};

#endif // G02_H
