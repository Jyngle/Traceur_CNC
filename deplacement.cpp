#include "deplacement.h"


float Deplacement::_dist_acceleration_XY_g00 = 555.555;
float Deplacement::_acceleration_XY = 100;
float Deplacement::_vitesse_XY_g00 = 333.33;

float Deplacement::_vitesse_Z_g00;  // mm/s
float Deplacement::_dist_acceleration_Z_g00;  // mm
float Deplacement::_acceleration_Z; // mm/s²


void Deplacement::set_info_abs(float X, float Y, float Z){

    X_abs = X;
    Y_abs = Y;
    Z_abs = Z;
}

void Deplacement::set_info_rel(float X, float Y, float Z){

    X_rel = X;
    Y_rel = Y;
    Z_rel = Z;
}


float Deplacement::get_Z(){
    return Z_abs;
}
