#include "deplacement.h"


float Deplacement::_dist_acceleration_g00 = 555.555;
float Deplacement::_acceleration = 100;
float Deplacement::_vitesse_g00 = 333.33;


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
