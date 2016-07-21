#include "parser.h"
#include "g00.h"
#include "g01.h"
#include "g02.h"
#include "g03.h"
#include "ligne.h"
#include "figure.h"
#include "finprogramme.h"
#include "config.h"

#include <string.h>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QThread>


void Parser::ReadInputFile()
{
    clean_file();
    parse_gcode_file(INPUT_GCODE,_ListeGcode, 0, 0, 0, 0);

    _ListeGcode.append(new FinProgramme());
    absolute_relative();
}

void Parser::compute_arc()
{
    QList<Ligne *>::iterator IT;
    QList<Ligne *>::iterator ITG01;

    int index1;
    int index2;
    G01 * DeplacementPrecedent;
    G01 * DeplacementActuel;
    float a, ap, b, bp;
    bool condition;
    bool CoordonneeIdentique = false;
    float xMoyen = 0, yMoyen = 0;
    int gg = 0;

    struct Coordonees{
        float x;
        float y;
        float z;
        int f;
    };

    Coordonees _CoordonneesIntersection;//utilisé dans la boucle for (valeur fausse à la sortie)
    Coordonees CoordonneesIntersection;//utilisé hors de la boucle for (valeur ok à la sortie)
    Coordonees CoordonneesIntersectionPrecedente;
    Coordonees PositionAvantCercle = {};
    Coordonees PositionFinCercle;
    Coordonees CoordonneesIntersectionRelatif;

    for(IT = _ListeGcode.begin(), index1 = 0; IT != _ListeGcode.end(); IT++, index1++)
    {

        if(!dynamic_cast<G01 *>(*IT))//Si objet n'est pas un G01
            continue;

        condition =  (IT+1 != _ListeGcode.end() && dynamic_cast<G01 *>(*(IT+1)));



        for(ITG01 = std::next(_ListeGcode.begin(), index1+1), index2 = index1+1; condition ; ITG01++, index2++)
        {

            DeplacementActuel = dynamic_cast<G01 *>(*ITG01);
            DeplacementPrecedent = dynamic_cast<G01 *>(*(ITG01 - 1));

            a = DeplacementActuel->get_coordonnees_droite_perpendiculaire()[0];
            b = DeplacementActuel->get_coordonnees_droite_perpendiculaire()[1];

            ap = DeplacementPrecedent->get_coordonnees_droite_perpendiculaire()[0];
            bp = DeplacementPrecedent->get_coordonnees_droite_perpendiculaire()[1];


            //calcul du point
            _CoordonneesIntersection.x = (bp - b) / (a - ap);
            _CoordonneesIntersection.y = ((a * bp) - (b * ap)) / (a - ap);

            if((CoordonneesIntersectionPrecedente.x - _CoordonneesIntersection.x) < 2 && (CoordonneesIntersectionPrecedente.x - _CoordonneesIntersection.x) > - 2)
                CoordonneeIdentique = true;
            if((CoordonneesIntersectionPrecedente.y - _CoordonneesIntersection.y) < 2 && (CoordonneesIntersectionPrecedente.y - _CoordonneesIntersection.y) > - 2)
                CoordonneeIdentique = CoordonneeIdentique && true;


            condition =  (ITG01+1 != _ListeGcode.end() && dynamic_cast<G01 *>(*(ITG01+1))) && CoordonneeIdentique;

            if(!CoordonneeIdentique)
            {
                index2--;
                ITG01--;
            }
            else
            {
                xMoyen += _CoordonneesIntersection.x;
                yMoyen += _CoordonneesIntersection.y;
                gg++;
                CoordonneesIntersection.x = _CoordonneesIntersection.x;
                CoordonneesIntersection.y = _CoordonneesIntersection.y;

            }

            CoordonneesIntersectionPrecedente.x = _CoordonneesIntersection.x;
            CoordonneesIntersectionPrecedente.y = _CoordonneesIntersection.y;
        }



        if(index2 - index1 > 4)
        {
            index1 = index1 - 2;
            IT = IT - 2;

            index2--;
            ITG01--;

            //Position avant cercle
            QList<Ligne *>::Iterator rIT = std::next(_ListeGcode.begin(), index1-1);
            for(rIT = std::next(_ListeGcode.begin(), index1-1); rIT != _ListeGcode.begin() && type_check(*rIT) != "Deplacement"; rIT--);
            rIT++;

            if(rIT == _ListeGcode.begin())
            {
                PositionAvantCercle.x = 0;
                PositionAvantCercle.y = 0;
                PositionAvantCercle.z = 0;
                PositionAvantCercle.f = 0;
            }
            else
            {
                PositionAvantCercle.x = dynamic_cast<Deplacement *>(*(rIT))->get_info_abs()[0];
                PositionAvantCercle.y = dynamic_cast<Deplacement *>(*(rIT))->get_info_abs()[1];
                PositionAvantCercle.z = dynamic_cast<Deplacement *>(*(rIT))->get_info_abs()[2];
            }

            //Position fin cercle : (*ITG01)->....
            PositionFinCercle.x = dynamic_cast<Deplacement *>(*ITG01)->get_info_abs()[0];
            PositionFinCercle.y = dynamic_cast<Deplacement *>(*ITG01)->get_info_abs()[1];
            PositionFinCercle.z = dynamic_cast<Deplacement *>(*ITG01)->get_info_abs()[2];



            //Détection sens horaire ou antihoraire
            //Recherche des 3 premiers point du cercle :

            float x1, x2,x3;
            float y1, y2, y3;

            x1 = dynamic_cast<Deplacement *>(_ListeGcode[index1])->get_info_abs()[0];
            x2 = dynamic_cast<Deplacement *>(_ListeGcode[index1+1])->get_info_abs()[0];
            x3 = dynamic_cast<Deplacement *>(_ListeGcode[index1+2])->get_info_abs()[0];

            y1 = dynamic_cast<Deplacement *>(*IT)->get_info_abs()[1];
            y2 = dynamic_cast<Deplacement *>(*(IT+1))->get_info_abs()[1];
            y3 = dynamic_cast<Deplacement *>(*(IT+2))->get_info_abs()[1];





            //Suppresion des éléments de la liste (ancien polygone)
            for(int i = 0; i <= index2 - index1; i++)
                _ListeGcode.removeAt(index1);

            //Calcul de la moyenne de l'intersection
            xMoyen = xMoyen / gg;
            yMoyen = yMoyen / gg;

            //Calcul de la surface que forme le tringle formé par ces 3 points
            //Si surface < 0 sens horaire
            if((((x1*y2-x2*y1)/2) + ((x2*y3-x3*y2)/2) + ((x3*y1-x1*y3)/2)) < 0)
            { //sens horaire

                //Calcul des coordonnes du centre du cercle en relatif par rapport au point de démarrage du cercle
                CoordonneesIntersectionRelatif.x = PositionAvantCercle.x - xMoyen;
                CoordonneesIntersectionRelatif.y = PositionAvantCercle.y - yMoyen;

                G02 * g02 = new G02(PositionFinCercle.x,PositionFinCercle.y,PositionFinCercle.z,CoordonneesIntersectionRelatif.x,CoordonneesIntersectionRelatif.y,PositionFinCercle.f);
                _ListeGcode.insert(index1, g02); //Peut être index1 + 1 ???

            }
            else //Sens antihoraire
            {
                //Calcul des coordonnes du centre du cercle en relatif par rapport au point de démarrage du cercle
                CoordonneesIntersectionRelatif.x =  xMoyen - PositionAvantCercle.x;
                CoordonneesIntersectionRelatif.y =  yMoyen - PositionAvantCercle.y;

                G03 * g03 = new G03(PositionFinCercle.x,PositionFinCercle.y,PositionFinCercle.z,CoordonneesIntersectionRelatif.x,CoordonneesIntersectionRelatif.y,PositionFinCercle.f);
                _ListeGcode.insert(index1, g03);
            }
            //Repositionnement de l'IT et de index1
            IT = std::next(_ListeGcode.begin(),index1 + 1);
            index1 = index1 + 1;

        }

    }


}

int Parser::ComputeTime()
{
    /*
    float Deplacement::_dist_acceleration_XY_g00 = 555.555;
        float Deplacement::_acceleration_XY = 100;
    float Deplacement::_vitesse_XY_g00 = 333.33;

        float Deplacement::_vitesse_Z_g00;  // mm/s
        float Deplacement::_dist_acceleration_Z_g00;  // mm
        float Deplacement::_acceleration_Z; // mm/s²*/

   QString strVitesseG00X,strVitesseG00Y,strVitesseG00Z,strAccZ, strAccX, strAccY;
   float VitesseG00X,VitesseG00Y,VitesseG00Z,AccX,AccY,AccZ;

   if(read_param_file("$110",strVitesseG00X)    ||
      read_param_file("$111",strVitesseG00Y)    ||
      read_param_file("$112",strVitesseG00Z)    ||
      read_param_file("$120",strAccX)           ||
      read_param_file("$121",strAccY)           ||
      read_param_file("$122",strAccZ))
   {
       throw QString("Impossible de lire les paramètres de vitesse et d'acélération de la machine");
   }

   VitesseG00X = strVitesseG00X.toFloat()/60;
   VitesseG00Y = strVitesseG00Y.toFloat()/60;
   VitesseG00Z = strVitesseG00Z.toFloat()/60;
   AccX = strAccX.toFloat();
   AccY = strAccY.toFloat();
   AccZ = strAccZ.toFloat();

   Deplacement::_vitesse_XY_g00 = (VitesseG00X + VitesseG00Y) / 2;//Grosse aproximation
   Deplacement::_vitesse_Z_g00 = VitesseG00Z;
   Deplacement::_dist_acceleration_Z_g00 = (AccZ/2)*powf(VitesseG00Z/AccZ,2);
   Deplacement::_acceleration_XY = (AccX + AccY) / 2; //Grosse aproximation
   Deplacement::_acceleration_Z = AccZ;


   float time = 0;

   for(QList<Ligne *>::iterator IT = _ListeGcode.begin(); IT != _ListeGcode.end(); IT++)
        if(type_check(*IT) == "Deplacement")
            time += dynamic_cast<Deplacement *>(*IT)->get_time();

   time += time*0.3;
   return (int)time;
}

void Parser::check_depacement()
{
    Position lim;
    read_ardware_limitFile(lim);

    QList<float> position;
    QList<Ligne *>::iterator IT;

    for(IT = _ListeGcode.begin(); IT != _ListeGcode.end(); IT++)
    {
        if(type_check(*IT) == "Deplacement")
        {
            position = dynamic_cast<Deplacement *>(*IT)->get_info_abs();

            if(position[0]>lim.X || position[1] > lim.Y || position[2] > lim.Z)
                throw QString("Une ou plusieurs limite(s) de la machine est(sont) dépassée(s)");
        }
    }
}

int Parser::read_param_file(QString key, QString & value)
{
    QString filename = QCoreApplication::applicationDirPath() + PARAM_GRBL;
    QFile Fichier_Param(filename);
    QTextStream stream_fichier_param(&Fichier_Param);
    QStringList Param;

    if(Fichier_Param.open(QIODevice::ReadOnly |QIODevice::Text))
        qDebug() << "Fichier param ouvert !" << endl;
    else
        throw QString("Impossible d'ouvrir le fichier de parametrage de la machine");

    while (!stream_fichier_param.atEnd())
    {
        QString ligne = stream_fichier_param.readLine();
        if (ligne.contains(key))
        {
            Param = ligne.split("=");
            Param = Param[1].split(" ");
            value = Param[0];
            Fichier_Param.close();
            return 0;

        }
    }

    Fichier_Param.close();
    return 1;
}

void Parser::read_ardware_limitFile(Position& abs)
{
    QString filename = QCoreApplication::applicationDirPath() + PARAM_GRBL;
    QFile Fichier_Param(filename);
    QTextStream stream_fichier_param(&Fichier_Param);
    bool x = false,y = false,z = false;
    QStringList Param;

    if(Fichier_Param.open(QIODevice::ReadOnly |QIODevice::Text))
        qDebug() << "Fichier param ouvert !" << endl;
    else
        throw QString("Impossible d'ouvrir le fichier de parametrage de la machine");

    while (!stream_fichier_param.atEnd())
    {
        QString ligne = stream_fichier_param.readLine();
        if (ligne.contains("$130"))
        {
            Param = ligne.split("=");
            abs.X = Param[1].toFloat();
            x = true;
        }
        if (ligne.contains("$131"))
        {
            Param = ligne.split("=");
            abs.Y = Param[1].toFloat();
            y = true;
        }
        if (ligne.contains("$132"))
        {
            Param = ligne.split("=");
            abs.Z = Param[1].toFloat();
            z = true;
        }
    }

    Fichier_Param.close();

    if(!(x && y && z))
        throw QString("Dimentions machine non renseignees, introuvables ou incomplètes");

}

void Parser::parse_gcode_file(QString name, QList<Ligne *> &__ListeGcode, float X, float Y, float Z, int F)
{

    QString filename = QCoreApplication::applicationDirPath() + name;
    QFile fichier_gcode(filename);
    QTextStream fichier_in(&fichier_gcode);

    if(fichier_gcode.open(QIODevice::ReadOnly |QIODevice::Text))
        qDebug() << "Fichier Gcode ouvert !" << endl;
    else
    {
        throw QString("Impossible d'ouvrir le fichier : " + name);
        qDebug() << "Impossible d'ouvrir le fichier : " << name << endl;
    }

    QString valTMP;

    while (!fichier_in.atEnd())
           {
              QString ligne = fichier_in.readLine();

              if (ligne.contains("G0") || ligne.contains("G1")){    //*********************************************************************

                  if(GetValue(ligne,"X",valTMP))
                      X = valTMP.toFloat();

                  if(GetValue(ligne,"Y",valTMP))
                      Y = valTMP.toFloat();

                  if(GetValue(ligne,"Z",valTMP))
                      Z = valTMP.toFloat();

                  //qDebug() << "test" << "X : " << X << " Y : " << Y << " Z : " << Z << endl;
                  if (ligne.contains("G0"))
                  {
                      G00 * g00 = new G00(X,Y,Z,F);
                      __ListeGcode.append(g00);
                  }
                  else
                  {
                      G01 * g01 = new G01(X,Y,Z,F);
                      __ListeGcode.append(g01);
                  }

              }

              if (ligne.contains("F")){//*********************************************************************
                    F = ligne.mid(ligne.indexOf("F")+1).simplified().toInt();
              }

              if (ligne.contains("G2") || ligne.contains("G3")){    //*********************************************************************

                  float X = 0;float Y = 0;float I = 0;float J = 0;

                  if(GetValue(ligne,"X",valTMP))
                      X = valTMP.toFloat();

                  if(GetValue(ligne,"Y",valTMP))
                      Y = valTMP.toFloat();

                  if(GetValue(ligne,"Z",valTMP))
                      Z = valTMP.toFloat();

                  if(GetValue(ligne,"I",valTMP))
                      I = valTMP.toFloat();

                  if(GetValue(ligne,"J",valTMP))
                      J = valTMP.toFloat();

                  //qDebug() << "X : " << X << " Y : " << Y << " I : " << I << " J : " << J << endl;

                  if (ligne.contains("G2"))
                  {
                      G02 * g02 = new G02(X,Y,Z,I,J,F);
                      __ListeGcode.append(g02);
                  }
                  else
                  {
                      G03 * g03 = new G03(X,Y,Z,I,J,F);
                      __ListeGcode.append(g03);
                  }

              }

              if (ligne.contains("SHAPE")){ //********************************************************************* 
                Figure * figure = new Figure();
                __ListeGcode.append(figure);
              }

              if (ligne.contains("OUTPUT")){//*********************************************************************
                  QStringList Param = ligne.split(" ");
                  output *OutPut = new output(Param[1], Param[2]);
                  __ListeGcode.append(OutPut);
              }

              if (ligne.contains("PAUSE") && GetValue(ligne,"PAUSE",valTMP)){ //*********************************************************************
                  pause *Pause = new pause(valTMP);
                  __ListeGcode.append(Pause);
              }

              if (ligne.contains("INPUT")){ //*********************************************************************
                  QStringList Param = ligne.split(" ");
                  input *InPut = new input(Param[1], Param[2]);
                  __ListeGcode.append(InPut);
              }
           }
    fichier_gcode.close();
}

void Parser::insert_macro_debut(QString FileNameMacro)
{
    insert_macro_at("MacroDebut",FileNameMacro,0);
}

void Parser::insert_macro_fin(QString FileNameMacro)
{
    insert_macro_at("MacroFin",FileNameMacro,_ListeGcode.size()-1);
}

void Parser::insert_macro_at(QString name, QString FileNameMacro, int Index)
{
    float X = 0, Y = 0, Z = 0;
    int F = 0;
    QList<float> CoordonneeABSXYZ;
    QList<Ligne *> ListeGcodeMacro;

    QList<Ligne *>::iterator IT;

    IT = _ListeGcode.begin();

    if(Index != 0)
    {
        //Recherche le dernier déplacement avant la macro pour extraire ses coordonnées
        //afin de repositionner la tête au même endroit à la fin de la macro
        for(IT += (Index-1);IT != _ListeGcode.begin() && type_check(*IT) != "Deplacement" ; IT--);

        if(IT != _ListeGcode.begin())//Si déplacement trouvé, extraction de ses coordonnées
        {
            CoordonneeABSXYZ = dynamic_cast<Deplacement*>(*IT)->get_info_abs();
            X = CoordonneeABSXYZ[0];
            Y = CoordonneeABSXYZ[1];
            Z = CoordonneeABSXYZ[2];
            F = dynamic_cast<Deplacement*>(*IT)->getF();
        }
    }

    //"Parsage" de la macro, initialisation des paramètres avec les paramère en amont de la macro
    parse_gcode_file(PATH_MACROS + FileNameMacro,ListeGcodeMacro,X,Y,Z,F);

    //Création de la macro
    macro * Macro = new macro(name,ListeGcodeMacro);

    //Ajout des positions de retour
    Macro->SetPositionRetour(X,Y,Z);

    //Insertion de la macro dans la liste de Gcode au bon index
    _ListeGcode.insert(Index,Macro);
}

void Parser::insert_macro_distance(QString FileNameMacro, float distance_min, float distance_max)
{
    float TailleDepuisDerniereOccurrence = 0;
    int Index = 0;
    QList<Ligne *>::iterator IT;

    for(IT = _ListeGcode.begin(), Index = 0; IT != _ListeGcode.end(); IT++, Index++) //Boucle sur toute la liste de Gcode
    {
        if(type_check(*IT) == "Figure")//Sifigure
        {
           if(TailleDepuisDerniereOccurrence >= distance_min) //Si avant une figure on peut insérer une macro
           {
               insert_macro_at("Distance",FileNameMacro, Index+1);
               IT = std::next(_ListeGcode.begin(),Index);
               TailleDepuisDerniereOccurrence = 0;
           }
        }
        else if(type_check(*IT) == "Deplacement")//Si déplacement
        {
            TailleDepuisDerniereOccurrence += dynamic_cast<Deplacement*>(*IT)->get_distance();

            if(TailleDepuisDerniereOccurrence > distance_max)//Si obligé d'insérer macro dans une figure
            {

                if(dynamic_cast<Deplacement*>(*(IT))->get_distance() > distance_max)//Si une ligne de Gcode est plus long que la distance max de la macro
                {
                    if((IT != _ListeGcode.begin() && type_check(*(IT-1)) != "Macro") || ((IT-1) != _ListeGcode.begin() && type_check(*(IT-1)) == "Macro"  && dynamic_cast<macro*>(*(IT-1))->_name != "Distance"))
                    {
                        //Si pas de macro identique précédent, ajout de 2 macros (une au début du long trajet
                        //et une à la fin du long trajet
                        insert_macro_at("Distance",FileNameMacro, Index);
                        IT = std::next(_ListeGcode.begin(),Index);
                        Index += 2;
                        IT += 2;
                        insert_macro_at("Distance",FileNameMacro, Index);
                        IT = std::next(_ListeGcode.begin(),Index);
                        TailleDepuisDerniereOccurrence = 0;
                    }
                    else
                    {
                        //Si une macro identique précède la ligne on ajoute
                        //Cette même macro seulement après la ligne (ligne encadré par deux macros)
                        Index++;
                        IT++;
                        insert_macro_at("Distance",FileNameMacro, Index);
                        IT = std::next(_ListeGcode.begin(),Index);
                        TailleDepuisDerniereOccurrence = 0;
                    }
                }
                else
                {
                    //Insertion macro classique :
                    //La distance max est atteinte par accumulation de plusieurs déplacements
                    insert_macro_at("Distance",FileNameMacro, Index);
                    IT = std::next(_ListeGcode.begin(),Index);
                    TailleDepuisDerniereOccurrence = 0;
                }
            }
        }
    }
}

void Parser::absolute_relative(){

    struct {
        float X, Y, Z;
    }CoordonneesABSprecedente;

    for(QList<Ligne *>::Iterator IT = _ListeGcode.begin(); IT != _ListeGcode.end(); IT++)
    {

        if(type_check(*IT) != "Deplacement")
            continue;

        CoordonneesABSprecedente.X = 0;
        CoordonneesABSprecedente.Y = 0;
        CoordonneesABSprecedente. Z = 0;

        if(IT != _ListeGcode.begin())
        {

            for(QList<Ligne *>::Iterator ITp = IT-1; ITp != _ListeGcode.begin(); ITp--)
            {
                if(type_check(*ITp) == "Deplacement")
                {
                    CoordonneesABSprecedente.X = dynamic_cast<Deplacement *>(*ITp)->get_info_abs()[0];
                    CoordonneesABSprecedente.Y = dynamic_cast<Deplacement *>(*ITp)->get_info_abs()[1];
                    CoordonneesABSprecedente.Z = dynamic_cast<Deplacement *>(*ITp)->get_info_abs()[2];
                    break;
                }
            }
        }

        dynamic_cast<Deplacement *>(*IT)->set_info_rel(CoordonneesABSprecedente.X - dynamic_cast<Deplacement *>(*IT)->get_info_abs()[0],
                                                       CoordonneesABSprecedente.Y - dynamic_cast<Deplacement *>(*IT)->get_info_abs()[1],
                                                       CoordonneesABSprecedente.Z - dynamic_cast<Deplacement *>(*IT)->get_info_abs()[2]);

        if(dynamic_cast<G01 *>(*IT))
            dynamic_cast<G01 *>(*IT)->set_coordonnee_precedentes(CoordonneesABSprecedente.X, CoordonneesABSprecedente.Y);

    }

    compute_taille_figure(_ListeGcode);
}

void Parser::compute_taille_figure(QList<Ligne *> liste_gcode){

            int indice = 0;
            float taille_fi = 0.;

            for(int i = 0;i<liste_gcode.size();i++){

                if (Parser::type_check(liste_gcode[i]) == "Figure"){
                    dynamic_cast<Figure *>(liste_gcode[indice])->set_taille(taille_fi);
                    taille_fi = 0;
                    indice = i;
                }
                if (Parser::type_check(liste_gcode[i]) == "Deplacement"){
                    taille_fi += dynamic_cast<Deplacement *>(liste_gcode[i])->get_distance();}

            }
            dynamic_cast<Figure *>(liste_gcode[indice])->set_taille(taille_fi);
}

void Parser::clean_file(){

    QList<QString> liste;
    QString filename_gcode_source = QCoreApplication::applicationDirPath() + INPUT_GCODE;
    QFile fichier_gcode_source(filename_gcode_source);
     QTextStream fichier_in(&fichier_gcode_source);
    fichier_gcode_source.open(QIODevice::ReadWrite | QIODevice::Text);

    /*QString filename_out = QCoreApplication::applicationDirPath() + GCODE_CLEANED;
    QFile out(filename_out);
    QTextStream stream_out(&out);
    out.open(QIODevice::WriteOnly | QIODevice::Text);*/

    if(fichier_in.atEnd())
    {

        throw QString("Fichier Gcode vide (" + QString(INPUT_GCODE) +")");
    }

    while (!fichier_in.atEnd()){
        liste.append(fichier_in.readLine());
    }

    fichier_gcode_source.resize(0);

    for(int i = 10;i<liste.size();i++){
        fichier_in << liste[i] << endl;
    }

    fichier_gcode_source.close();

    //for(int i = 10;i<liste.size();i++){
    //    stream_out << liste[i] << endl;
    //}

}

void Parser::AjoutMacros(){

    QString filename_corres = QCoreApplication::applicationDirPath() + CORRESPONDANCE;
    QFile fichier_corres(filename_corres);
    QTextStream fichier_in(&fichier_corres);

    fichier_corres.open(QIODevice::ReadOnly | QIODevice::Text);
    QString ligne;

    while (!fichier_in.atEnd()){
        ligne = fichier_in.readLine();

        if(ligne.contains("Distance"))
        {
            QStringList liste = ligne.split(" ");        
            insert_macro_distance(liste[4],liste[2].toFloat(),liste[3].toFloat());
        }
        else if(ligne.contains("Début"))
        {
             QStringList liste = ligne.split(" ");
             insert_macro_debut(liste[2]);
        }
        else if(ligne.contains("Fin"))
        {
             QStringList liste = ligne.split(" ");
             insert_macro_fin(liste[2]);
        }

    }
}

void Parser::WriteOutputFile(){

     QString filename_out = QCoreApplication::applicationDirPath() + OUTPUT_GCODE;
     QFile out(filename_out);
     QTextStream stream_out(&out);
     out.open(QIODevice::WriteOnly | QIODevice::Text);

     for (int i = 0;i<_ListeGcode.size();i++){
         stream_out << _ListeGcode[i]->gcode_ligne() << endl;
     }
     out.close();
}

int Parser::GetValue(QString ligne, QString Key, QString &Value)
{
    if(!ligne.contains(Key)) {return 0;}

    ligne = ligne.simplified();
    ligne = ligne.replace(" ","");

    QStringList Param = ligne.split(Key);
    int i;
    const char* c = Param[1].toStdString().c_str();

    for(i = 0; (c[i] >= 48 && c[i] <= 57) || (c[i] == 46) || (c[i] == 45); i++);

    QString tmp = QString(c);
    Value = tmp.mid(0, i);
    if(Value.isEmpty())
        return 0;


return 1;
}

QString Parser::type_check(Ligne *elt){
    if (dynamic_cast<Deplacement *>(elt)){return "Deplacement";}
    if (dynamic_cast<Figure *>(elt)){return "Figure";}
    if (dynamic_cast<macro *>(elt)){return "Macro";}

        else return "inconnu";
}

