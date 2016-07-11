#ifndef DEPLACEMENT_H
#define DEPLACEMENT_H

#include "ligne.h"
#include <QList>




class Deplacement : public Ligne{

public:
    virtual QList<float> get_info_abs() = 0;
    virtual QList<float> get_info_rel() = 0;
    virtual float get_distance() = 0;
    void set_info_abs(float X, float Y, float Z);
    void set_info_rel(float X, float Y, float Z);
    virtual QString gcode_ligne() = 0;
    float get_Z();
    virtual float get_time() = 0;
    int getF(){return F;}



protected:


    static float _vitesse_g00;  // mm/s
    static float _dist_acceleration_g00;  // mm
    static float _acceleration; // mm/s²

    float _dist_acceleration;
    float X_abs = 0;
    float Y_abs = 0;
    float X_rel = 0;
    float Y_rel = 0;
    float F = 0;
    float Z_abs = 0;
    float Z_rel = 0;
};




#endif // DEPLACEMENT_H
