#ifndef ARQUEDECERCLE_H
#define ARQUEDECERCLE_H

#include "deplacement.h"

class ArqueDeCercle : public Deplacement{

public :
    //ArqueDeCercle(float _X, float _Y, float _Z, float _I, float _J, float _F);
    virtual QList<float> get_info_abs();
    virtual QList<float> get_info_rel();
    virtual float get_distance();
    virtual QString gcode_ligne() = 0;
    virtual float get_time();

protected:
    long double I = 0;
    long double J = 0;
};

#endif // ARQUEDECERCLE_H
