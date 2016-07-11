#include "g01.h"
#include <QDebug>

G01::G01(float _X, float _Y, float _Z, float _F){

    X_abs = _X;
    Y_abs = _Y;
    Z_abs = _Z;
    F = _F;
    _dist_acceleration = (_acceleration/2) * powf((F/60)/_acceleration, 2);
}

float G01::get_time()
{
    if(X_rel > 0 && Y_rel > 0)
    {
        float dist = sqrt(pow(X_rel,2) + pow(Y_rel,2));

        if(_dist_acceleration_g00*2 > dist) //Si deux phases : accélération puis décélération
        {
            return (sqrt(dist/_acceleration)) * 2;
        }
        else//Si trois phases : accélération, maintien, décélération
        {
            return (sqrt((2*_dist_acceleration_g00)/_acceleration)) * 2 + ((dist - _dist_acceleration_g00)/F);
        }
    }
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
