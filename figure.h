#ifndef FIGURE_H
#define FIGURE_H


#include "ligne.h"

class Figure : public Ligne{

public:
    virtual QString gcode_ligne();
    void set_taille(float _taille);
    float get_taille();

private:
    float taille_figure;

};//test


#endif // FIGURE_H
