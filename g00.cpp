#include "g00.h"
#include <QDebug>

G00::G00(float _X, float _Y, float _Z, float _F){

    X_abs = _X;
    Y_abs = _Y;
    Z_abs = _Z;
    F = _F;
    _dist_acceleration = (_acceleration/2) * powf((F/60)/_acceleration, 2);
}

 float G00::get_time()
 {
     if(X_rel > 0 && Y_rel > 0)
     {
         float dist = sqrt(pow(X_rel,2) + pow(Y_rel,2));

         if(_dist_acceleration_g00 * 2 > dist) //Si deux phases : accélération puis décélération
         {
             return (sqrt(dist/_acceleration)) * 2;
         }
         else//Si trois phases : accélération, maintien, décélération
         {
             return (sqrt((2*_dist_acceleration_g00)/_acceleration)) * 2 + ((dist - _dist_acceleration_g00)/_vitesse_g00);
         }
     }
     return 0;
 }


QList<float> G00::get_info_abs(){
    QList<float> liste;
    //qDebug() << "X = " <<  QString::number(X) << "Y = " << QString::number(Y) << "Z = " << QString::number(Z) << "F = " << QString::number(F);
    liste.append(X_abs);
    liste.append(Y_abs);
    liste.append(Z_abs);
    return liste;
}

QList<float> G00::get_info_rel(){
    QList<float> liste;
    //qDebug() << "X = " <<  QString::number(X) << "Y = " << QString::number(Y) << "Z = " << QString::number(Z) << "F = " << QString::number(F);
    liste.append(X_rel);
    liste.append(Y_rel);
    return liste;
}



float G00::get_distance(){
//crayon pas deposé == pas de distance
    return 0;
}


QString G00::gcode_ligne(){
    QString ligne;
    //qDebug() << "G00 X "+ QString::number(X_abs) + " Y " + QString::number(Y_abs) + " Z " + QString::number(Z) + " F " + QString::number(F);
    ligne = "G00 X " + QString::number(X_abs) + " Y " + QString::number(Y_abs)  + " Z " + QString::number(Z_abs) +  " F " + QString::number(F);
    return ligne;

}
