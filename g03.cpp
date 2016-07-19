#include "g03.h"
#define PI 3.14159265358979323846
#include <QDebug>

G03::G03(float _X, float _Y, float _Z, float _I, float _J, float _F){

    X_abs = _X;
    Y_abs = _Y;
    Z_abs = _Z;
    I = _I;
    J = _J;
    F = _F;
    _dist_acceleration_XY = (_acceleration_XY/2) * powf((F/60)/_acceleration_XY, 2);
}



QString G03::gcode_ligne(){

    //qDebug() << "G02 X " + QString::number(X) + " Y " + QString::number(Y) + " I " + QString::number(I) + " J " + QString::number(J) + " F " + QString::number(F);

    return "G03 X " + QString::number(X_abs) + " Y "  + QString::number(Y_abs) + " Z "  + QString::number(Z_abs) + " I " + QString::number(I) + " J " + QString::number(J) + " F " + QString::number(F);

}
