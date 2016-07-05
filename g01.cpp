#include "g01.h"
#include <QDebug>

G01::G01(float _X, float _Y, float _Z, float _F){

    X_abs = _X;
    Y_abs = _Y;
    Z = _Z;
    F = _F;
}

QList<float> G01::get_info_abs(){
    QList<float> liste;
    //qDebug() << "X = " <<  QString::number(X) << "Y = " << QString::number(Y) << "Z = " << QString::number(Z) << "F = " << QString::number(F);
    liste.append(X_abs);
    liste.append(Y_abs);
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

void G01::set_info_abs(float _X, float _Y){

    X_abs = _X;
    Y_abs = _Y;
}

void G01::set_info_rel(float _X, float _Y){

    X_rel = _X;
    Y_rel = _Y;
}

float G01::get_Z(){
    return Z;
}

QString G01::gcode_ligne(){
    QString ligne;
    //qDebug() << "G01 X "+ QString::number(X_abs) + " Y " + QString::number(Y_abs) + " Z "  + QString::number(Z) + " F " + QString::number(F);
    if (X_rel == 0 && Y_rel == 0 && Z != 0){ligne = "G01 Z " + QString::number(Z) + " F " + QString::number(F);}
    else ligne = "G01 X " + QString::number(X_abs) + " Y " + QString::number(Y_abs) +" F " + QString::number(F);
    return ligne;

}
