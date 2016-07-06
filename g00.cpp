#include "g00.h"
#include <QDebug>

G00::G00(float _X, float _Y, float _Z, float _F){

    X_abs = _X;
    Y_abs = _Y;
    Z_abs = _Z;
    F = _F;
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

void G00::set_info_abs(float _X, float _Y){

    X_abs = _X;
    Y_abs = _Y;
}

void G00::set_info_rel(float _X, float _Y){

    X_rel = _X;
    Y_rel = _Y;
}

float G00::get_distance(){
//crayon pas deposé == pas de distance
    return 0;
}

float G00::get_Z(){
    return Z_abs;
}

QString G00::gcode_ligne(){
    QString ligne;
    //qDebug() << "G00 X "+ QString::number(X_abs) + " Y " + QString::number(Y_abs) + " Z " + QString::number(Z) + " F " + QString::number(F);
    ligne = "G00 X " + QString::number(X_abs) + " Y " + QString::number(Y_abs)  + " Z " + QString::number(Z_abs) +  " F " + QString::number(F);
    return ligne;

}
