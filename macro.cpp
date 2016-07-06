#include "macro.h"


macro::macro(QString name, QList<Ligne *> ListeGcodeMacro): _ListeGcodeMacro(ListeGcodeMacro), _name(name)
{

}


QString macro::gcode_ligne()
{
   QList<Ligne*>::iterator itListe;
   QString output;

    for (itListe = _ListeGcodeMacro.begin(); itListe != _ListeGcodeMacro.end() ; itListe++){
        output += (*itListe)->gcode_ligne() + "/n";
    }

    return output;
}


void macro::SetPositionRetour(float X, float Y, float Z)
{
    float ZBeforReturnToPosition = Z;
    QList<Ligne*>::iterator itListe;

    //Recherche si une ancienne position Z a été donnée durant la macro
    //Si trouveé, retour à la position X, Y puis desecnte de l'axe Z
    //Si aucune position Z donnée durant la macro, retour directe (mouvements en X, Y, Z simultanés)
    for(itListe = _ListeGcodeMacro.end(); itListe != _ListeGcodeMacro.begin(); itListe--)
    {
       if(dynamic_cast<Deplacement *>(*itListe))
       {
           ZBeforReturnToPosition = dynamic_cast<Deplacement *>(*itListe)->get_Z();
           break;
       }
    }

    G00 *g00_1 = new G00(X,Y,ZBeforReturnToPosition,0);
    G00 *g00_2 = new G00(X,Y,Z,0);

    _ListeGcodeMacro.append(g00_1);
    _ListeGcodeMacro.append(g00_2);

}
