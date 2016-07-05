#ifndef G01_H
#define G01_H

#include "deplacement.h"

class G01 : public Deplacement{

public :
    G01(float _X, float _Y, float _Z, float _F);
    virtual QList<float> get_info_abs();
    virtual QList<float> get_info_rel();
    virtual float get_distance();
    virtual QString gcode_ligne();
    virtual void set_info_abs(float _X, float _Y);
    virtual void set_info_rel(float _X, float _Y);
    virtual float get_Z();

private:
    float Z = 0;
};

#endif // G01_H
