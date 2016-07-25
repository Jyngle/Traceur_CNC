#include "g02.h"
#define PI 3.14159265358979323846
#include <QDebug>


G02::G02(float _X, float _Y, float _Z, float _I, float _J, float _F){

    X_abs = _X;
    Y_abs = _Y;
    Z_abs = _Z;
    I = _I;
    J = _J;
    F = _F;
    _dist_acceleration_XY = (_acceleration_XY/2) * powf((F/60)/_acceleration_XY, 2);
}



QString G02::gcode_ligne(){

    //qDebug() << "G02 X " + QString::number(X) + " Y " + QString::number(Y) + " I " + QString::number(I) + " J " + QString::number(J) + " F " + QString::number(F);
    //return "G02 X " + QString::number(X_abs) + " Y "  + QString::number(Y_abs) + " Z "  + QString::number(Z_abs) + " R " + QString::number(sqrt(pow(X_abs-I,2)+pow(Y_abs-J,2))) + " F " + QString::number(F);
    return "G02 X " + QString::number((float)X_abs) + " Y "  + QString::number((float)Y_abs) + " Z "  + QString::number((float)Z_abs) + " I " + QString::number((float)((abs(I)<0.00001)?0:I)) + " J " + QString::number((float)((abs(J)<0.00001)?0:J)) + " F " + QString::number(F);

}
