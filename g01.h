#ifndef G01_H
#define G01_H

#include "deplacement.h"

class G01 : public Deplacement{

public :
    G01(float _X, float _Y, float _Z, float _F);
    virtual QList<float> get_info_abs();
    QList<float> get_info_abs_prec();
    virtual QList<float> get_info_rel();
    virtual float get_distance();
    virtual QString gcode_ligne();
    virtual float get_time();

    void set_coordonnee_precedentes(float X, float Y);

    QList<float> get_coordonnees_droite_perpendiculaire();




private:

    float X_abs_precedente;
    float Y_abs_precedente;

    float coef_droite_a;
    float coef_droite_b;

    float coef_droite_perpendiculaire_a;
    float coef_droite_perpendiculaire_b;

    bool perpendiculaireEstVerticale = false;

};

#endif // G01_H
