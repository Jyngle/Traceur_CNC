#ifndef DEPLACEMENT_H
#define DEPLACEMENT_H

#include "ligne.h"
#include <QList>

class Deplacement : public Ligne{

public:
    virtual QList<float> get_info_abs() = 0;
    virtual QList<float> get_info_rel() = 0;
    virtual float get_distance() = 0;
    virtual void set_info_abs(float _X, float _Y) = 0;
    virtual void set_info_rel(float _X, float _Y) = 0;
    virtual QString gcode_ligne() = 0;
    virtual float get_Z() = 0;
    int getF(){return F;}



protected:
    float X_abs = 0;
    float Y_abs = 0;
    float X_rel = 0;
    float Y_rel = 0;
    float F = 0;
    float Z_abs = 0;
};

#endif // DEPLACEMENT_H
