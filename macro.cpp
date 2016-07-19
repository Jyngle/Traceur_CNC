#include "macro.h"


macro::macro(QString name, QList<Ligne *> ListeGcodeMacro): _ListeGcodeMacro(ListeGcodeMacro), _name(name)
{

}


QString macro::gcode_ligne()
{
   QList<Ligne*>::iterator itListe;
   QString output;

   output += " \n(* Début Macro " + _name + " *) \n";

    for (itListe = _ListeGcodeMacro.begin(); itListe != _ListeGcodeMacro.end() ; itListe++){
        output += (*itListe)->gcode_ligne() + "\n";
    }

    output += "(* Fin Macro " + _name + " *) \n";
    return output;
}


void macro::SetPositionRetour(float X, float Y, float Z)
{
    if(_name == "Fin") //Pas de position de retour si la macro est de type macro Fin
        return;


    float ZBeforReturnToPosition = Z;
    QList<Ligne*>::reverse_iterator rIT;

    //Recherche si une ancienne position Z a été donnée durant la macro
    //Si trouveé, retour à la position X, Y puis desecnte de l'axe Z
    //Si aucune position Z donnée durant la macro, retour directe (mouvements en X, Y, Z simultanés)
    for(rIT = _ListeGcodeMacro.rbegin(); rIT != _ListeGcodeMacro.rend(); rIT++)
    {
       if(dynamic_cast<Deplacement *>(*rIT))
       {
           ZBeforReturnToPosition = dynamic_cast<Deplacement *>(*rIT)->get_Z();
           break;
       }
    }



    G00 *g00_1 = new G00(X,Y,ZBeforReturnToPosition,0);
    _ListeGcodeMacro.append(g00_1);

    if(ZBeforReturnToPosition != Z)
    {
        G00 *g00_2 = new G00(X,Y,Z,0);
        _ListeGcodeMacro.append(g00_2);
    }

}
