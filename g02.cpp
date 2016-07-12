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

float G02::get_time()
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
QList<float> G02::get_info_abs(){
    QList<float> liste;
    //qDebug() << "X = " <<  QString::number(X) << "Y = " << QString::number(Y) << "I = " << QString::number(I) << "J = " << QString::number(J) << "F = " << QString::number(F);
    liste.append(X_abs);
    liste.append(Y_abs);
    liste.append(Z_abs);
    return liste;
}

QList<float> G02::get_info_rel(){
    QList<float> liste;
    //qDebug() << "X = " <<  QString::number(X) << "Y = " << QString::number(Y) << "I = " << QString::number(I) << "J = " << QString::number(J) << "F = " << QString::number(F);
    liste.append(X_rel);
    liste.append(Y_rel);
    return liste;
}






float G02::get_distance(){

    float R = sqrt(pow(I,2) + pow(J,2));
    //qDebug() <<"rayon"<< QString::number(R);
    float D = sqrt(pow(X_rel,2) + pow(Y_rel,2));
    //qDebug() <<"distance1"<< QString::number(D);

    float tmp = (pow(D,2) / (2*pow(R,2)));
    if (tmp>2){tmp = 2;}
    //qDebug() << "tmp" << QString::number(tmp);
    float angle = acos(1 -  tmp); //Al-Kashi
    //qDebug() <<"angle"<< QString::number(angle);
    //qDebug() << "res" << QString::number(pow(D,2) / (2*pow(R,2)));
    float distance = angle * R;
    //qDebug() <<"distance"<< QString::number(distance);

    return distance;

}

QString G02::gcode_ligne(){

    //qDebug() << "G02 X " + QString::number(X) + " Y " + QString::number(Y) + " I " + QString::number(I) + " J " + QString::number(J) + " F " + QString::number(F);

    return "G02 X " + QString::number(X_abs) + " Y "  + QString::number(Y_abs) + " Z "  + QString::number(Z_abs) + " I " + QString::number(I) + " J " + QString::number(J) + " F " + QString::number(F);

}
