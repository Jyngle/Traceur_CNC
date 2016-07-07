#include "parser.h"
#include "g00.h"
#include "g01.h"
#include "g02.h"
#include "ligne.h"
#include "figure.h"
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QThread>



void Parser::ReadInputFile(QString FileNameIN)
{
    clean_file(FileNameIN);
    parse_gcode_file("gcode.nc",_ListeGcode, 0, 0, 0, 0);
    absolute_relative();
}




void Parser::parse_gcode_file(QString name, QList<Ligne *> &__ListeGcode, float X, float Y, float Z, int F)
{

    QString filename = QCoreApplication::applicationDirPath() + "/" + name;
    QFile fichier_gcode(filename);
    QTextStream fichier_in(&fichier_gcode);

    if(fichier_gcode.open(QIODevice::ReadOnly |QIODevice::Text))
        qDebug() << "Fichier Gcode ouvert !" << endl;
    else
        qDebug() << "Probleme a la lecture du fichier" << endl;

    //int F = 0;//Vitesse par default
    //float X = 0, Y = 0, Z = 0;
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

//*********************************************************


void Parser::insert_macro_at(QString FileNameMacro, int Index)
{
    float X = 0, Y = 0, Z = 0;
    int F = 0;
    QList<float> CoordonneeABSXYZ;
    QList<Ligne *> ListeGcodeMacro;

    QList<Ligne *>::iterator IT;

    IT = _ListeGcode.begin();

    //Recherche le dernier déplacement avant la macro pour extraire ses coordonnées
    //afin de repositionner la tête au même endroit à la fin de la macro
    for(IT += (Index-1); !(!(IT == _ListeGcode.begin()) && !(type_check(*IT) != "Deplacement")); IT--);

    if(IT != _ListeGcode.begin())//Si déplacement trouvé, extraction de ses coordonnées
    {
        CoordonneeABSXYZ = dynamic_cast<Deplacement*>(*IT)->get_info_abs();
        X = CoordonneeABSXYZ[0];
        Y = CoordonneeABSXYZ[1];
        Z = CoordonneeABSXYZ[2];
        F = dynamic_cast<Deplacement*>(*IT)->getF();
    }

    //"Parsage" de la macro, initialisation des paramètres avec les paramère en amont de la macro
    parse_gcode_file(FileNameMacro,ListeGcodeMacro,X,Y,Z,F);

    //Création de la macro distance
    macro * Macro = new macro("Distance",ListeGcodeMacro);

    //Ajout des positions de retour
    Macro->SetPositionRetour(X,Y,Z);

    //Insertion de la macro dans la liste de Gcode au bon index
    _ListeGcode.insert(Index,Macro);
}

//*********************************************************
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
               insert_macro_at(FileNameMacro, Index+1);
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
                    if(((IT-1) != _ListeGcode.begin() && type_check(*(IT-1)) != "Macro") || ((IT-1) != _ListeGcode.begin() && type_check(*(IT-1)) == "Macro"  && dynamic_cast<macro*>(*(IT-1))->_name != "Distance"))
                    {
                        //Si pas de macro identique précédent, ajout de 2 macros (une au début du long trajet
                        //et une à la fin du long trajet
                        insert_macro_at(FileNameMacro, Index);
                        IT = std::next(_ListeGcode.begin(),Index);
                        Index += 2;
                        IT += 2;
                        insert_macro_at(FileNameMacro, Index);
                        IT = std::next(_ListeGcode.begin(),Index);
                        TailleDepuisDerniereOccurrence = 0;
                    }
                    else
                    {
                        //Si une macro identique précède la ligne on ajoute
                        //Cette même macro seulement après la ligne (ligne encadré par deux macros)
                        Index++;
                        IT++;
                        insert_macro_at(FileNameMacro, Index);
                        IT = std::next(_ListeGcode.begin(),Index);
                        TailleDepuisDerniereOccurrence = 0;
                    }
                }
                else
                {
                    //Insertion macro classique :
                    //La distance max est atteinte par accumulation de plusieurs déplacements
                    insert_macro_at(FileNameMacro, Index);
                    IT = std::next(_ListeGcode.begin(),Index);
                    TailleDepuisDerniereOccurrence = 0;
                }
            }
        }
    }
}
//*********************************************************

//*********************************************************

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

void Parser::clean_file(QString name){

    QList<QString> liste;
    QString filename_gcode_source = QCoreApplication::applicationDirPath() + "/" + name;
    QFile fichier_gcode_source(filename_gcode_source);
    QTextStream fichier_in(&fichier_gcode_source);
    fichier_gcode_source.open(QIODevice::ReadWrite | QIODevice::Text);

    QString filename_out = QCoreApplication::applicationDirPath() + "/gcode.nc";
    QFile out(filename_out);
    QTextStream stream_out(&out);
    out.open(QIODevice::WriteOnly | QIODevice::Text);


    while (!fichier_in.atEnd()){
        liste.append(fichier_in.readLine());
    }
    fichier_gcode_source.close();

    for(int i = 10;i<liste.size();i++){
        stream_out << liste[i] << endl;
    }

}

void Parser::AjoutMacros(QString FileNameCorespondance){

    QString filename_corres = QCoreApplication::applicationDirPath() + "/" + FileNameCorespondance;
    QFile fichier_corres(filename_corres);
    QTextStream fichier_in(&fichier_corres);

    fichier_corres.open(QIODevice::ReadOnly | QIODevice::Text);
    QString ligne;

    while (!fichier_in.atEnd()){
        ligne = fichier_in.readLine();

        if(ligne.contains("Distance")){
            QStringList liste = ligne.split(" ");        
            insert_macro_distance(liste[4],liste[2].toFloat(),liste[3].toFloat());
        }

    }

}

void Parser::WriteOutputFile(QString FileNameOUT){

     QString filename_out = QCoreApplication::applicationDirPath() + "/" + FileNameOUT;
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
    QStringList Param = ligne.split(" ");

    if(!ligne.contains(Key)) {return 0;}
    int j=0;

    for(int i=0; i<Param.size(); i++)
    {
        if(Param[i] == Key)// Si la clef est trouvée
        {
            for(j=0; i + 1 + j <= Param.size() && Param[i+1+j] == ""; j++);//supression des espaces après la clef

            if(i+1+j > Param.size()+1) //Pas de valeur trouvées
                return 0;
            else
                Value = Param[i+1+j];//Récupération de la valeur

            return 1; //test
        }
    }
return 0;
}

QString Parser::type_check(Ligne *elt){
    if (dynamic_cast<Deplacement *>(elt)){return "Deplacement";}
    if (dynamic_cast<Figure *>(elt)){return "Figure";}
    if (dynamic_cast<macro *>(elt)){return "Macro";}
        else return "inconnu";
}

