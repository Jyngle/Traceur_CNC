#include "figure.h"
#include <QDebug>

QString Figure::gcode_ligne(){
    //qDebug() << "(* Nouvelle figure *)";

    return  "\n(* ---------------- Nouvelle figure ---------------- *)";

}

float Figure::get_taille(){
    return taille_figure;
}

void Figure::set_taille(float _taille){
    taille_figure = _taille;
}
