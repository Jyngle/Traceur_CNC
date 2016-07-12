#include "g01.h"
#include <QDebug>

G01::G01(float _X, float _Y, float _Z, float _F){

    X_abs = _X;
    Y_abs = _Y;
    Z_abs = _Z;
    F = _F;
    _dist_acceleration_XY = (_acceleration_XY/2) * powf((F/60)/_acceleration_XY, 2);
    _dist_acceleration_Z = (_acceleration_Z/2) * powf((F/60)/_acceleration_Z, 2);
}

float G01::get_time()
{
    float dist_accelerationXY = _dist_acceleration_XY;


    if(Z_rel != 0 && _dist_acceleration_Z_g00 > _dist_acceleration_XY_g00)
    {
       dist_accelerationXY = _dist_acceleration_Z_g00;
    }

    float dist = sqrt(pow(X_rel,2) + pow(Y_rel,2));

    if(dist_accelerationXY * 2 > dist) //Si deux phases : accélération puis décélération
    {
        time = (sqrt(dist/_acceleration_XY)) * 2;
        return (sqrt(dist/_acceleration_XY)) * 2;
    }
    else//Si trois phases : accélération, maintien, décélération
    {
        time = (sqrt((2*dist_accelerationXY)/_acceleration_XY)) * 2 + ((dist - dist_accelerationXY)/F);
        return (sqrt((2*dist_accelerationXY)/_acceleration_XY)) * 2 + ((dist - dist_accelerationXY)/F);
    }


    time = 0;
    return 0;
}

QList<float> G01::get_info_abs(){
    QList<float> liste;
    //qDebug() << "X = " <<  QString::number(X) << "Y = " << QString::number(Y) << "Z = " << QString::number(Z) << "F = " << QString::number(F);
    liste.append(X_abs);
    liste.append(Y_abs);
    liste.append(Z_abs);
    return liste;
}

QList<float> G01::get_info_rel(){
    QList<float> liste;
    //qDebug() << "X = " <<  QString::number(X) << "Y = " << QString::number(Y) << "Z = " << QString::number(Z) << "F = " << QString::number(F);
    liste.append(X_rel);
    liste.append(Y_rel);
    return liste;
}

float G01::get_distance(){

    return sqrt(pow(X_rel,2) + pow(Y_rel,2));

}




QString G01::gcode_ligne(){
    QString ligne;
    //qDebug() << "G01 X "+ QString::number(X_abs) + " Y " + QString::number(Y_abs) + " Z "  + QString::number(Z) + " F " + QString::number(F);
    ligne = "G01 X " + QString::number(X_abs) + " Y " + QString::number(Y_abs)  + " Z " + QString::number(Z_abs) +  " F " + QString::number(F);
    return ligne;

}
