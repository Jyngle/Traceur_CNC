#include "parser.h"
#include "g00.h"
#include "g01.h"
#include "g02.h"
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
            Param = Param[1].split(" ");
            abs.X = Param[0].toFloat();
            x = true;
        }
        if (ligne.contains("$131"))
        {
            Param = ligne.split("=");
            Param = Param[1].split(" ");
            abs.Y = Param[0].toFloat();
            y = true;
        }
        if (ligne.contains("$132"))
        {
            Param = ligne.split("=");
            Param = Param[1].split(" ");
            abs.Z = Param[0].toFloat();
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

              if (ligne.contains("G2")){    //*********************************************************************

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
                  G02 * g02 = new G02(X,Y,Z,I,J,F);
                  __ListeGcode.append(g02);
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

    QList<Ligne*> liste_dep;
    for(int i = 0;i<_ListeGcode.size();i++){
        if (Parser::type_check(_ListeGcode[i]) == "Deplacement"){
            liste_dep.append(_ListeGcode[i]);
        }
    } //recupere une liste avec que les deplacements

    float X_new,Y_new,X_actu,Y_actu,X_prec,Y_prec;
    QList<float> liste_new;

        liste_new.append(dynamic_cast<Deplacement *>(liste_dep[0])->get_info_abs()[0]);
        liste_new.append(dynamic_cast<Deplacement *>(liste_dep[0])->get_info_abs()[1]);

        for (int i = 1;i< liste_dep.size();i++){

            X_actu = dynamic_cast<Deplacement *>(liste_dep[i])->get_info_abs()[0];
            Y_actu = dynamic_cast<Deplacement *>(liste_dep[i])->get_info_abs()[1];

            X_prec = dynamic_cast<Deplacement *>(liste_dep[i-1])->get_info_abs()[0];
            Y_prec = dynamic_cast<Deplacement *>(liste_dep[i-1])->get_info_abs()[1];

            X_new = X_actu - X_prec;
            Y_new = Y_actu - Y_prec;

            liste_new.append(X_new);
            liste_new.append(Y_new);
        }

        float X_rel,Y_rel;
        for (int i = 0;i< liste_dep.size();i++){
            X_rel = liste_new.takeFirst();
            Y_rel = liste_new.takeFirst();
            dynamic_cast<Deplacement *>(liste_dep[i])->set_info_rel(X_rel,Y_rel);
        }

        for(int i = 0;i<_ListeGcode.size();i++){
                    if (Parser::type_check(_ListeGcode[i]) == "Deplacement"){
                        _ListeGcode.replace(i,liste_dep.takeFirst());
                    }
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

