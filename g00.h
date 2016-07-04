#ifndef G00_H
#define G00_H

#include "deplacement.h"

class G00 : public Deplacement{

public :
    G00(float _X, float _Y, float _Z, float _F);
    virtual QList<float> get_info();
    virtual float get_distance();
    virtual QString gcode_ligne();
    virtual void set_info_abs(float _X, float _Y);
    virtual void set_info_rel(float _X, float _Y);

private:
    float Z = 0;
};

#endif // G00_H
