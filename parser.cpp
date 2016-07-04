#include "parser.h"
#include "g00.h"
#include "g01.h"
#include "g02.h"
#include "ligne.h"
#include "figure.h"
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

QList<Ligne *> Parser::parse_gcode(QString name)
{

    QString filename = QCoreApplication::applicationDirPath() + "/" + name;
    QFile fichier_gcode(filename);
    QTextStream fichier_in(&fichier_gcode);

    if(fichier_gcode.open(QIODevice::ReadOnly |QIODevice::Text)){

    qDebug() << "Fichier Gcode ouvert !" << endl;
    }
    else {qDebug() << "Probleme a la lecture du fichier";}

    int F = 0;
    QList<Ligne*> liste;
    while (!fichier_in.atEnd())
           {
              QString ligne = fichier_in.readLine();

              if (ligne.contains("G0")){
                  int indiceX = ligne.indexOf("X");
                  int indiceY = ligne.indexOf("Y");
                  int indiceZ = ligne.indexOf("Z");
                  float X = 0;float Y = 0;float Z = 0;

                  if (indiceX != -1){
                    X = ligne.mid(indiceX+1,8).simplified().toFloat();
                    //qDebug() << "X = " << ligne.mid(indiceX+1,8).simplified();
                    }
                  if (indiceY != -1){
                    Y = ligne.mid(indiceY+1,8).simplified().toFloat();
                    //qDebug() << "Y = "<< ligne.mid(indiceY+1,8).simplified();
                    }
                  if (indiceZ != -1){
                    Z = ligne.mid(indiceZ+1,8).simplified().toFloat();
                    //qDebug() << "Z = "<< ligne.mid(indiceZ+1,8).simplified();
                    }
                  //qDebug() << QString::number(X) << QString::number(Y) << QString::number(Z) << endl;
                  G00 * g00 = new G00(X,Y,Z,F);
                  liste.append(g00);
              }


              if (ligne.contains("G1")){
                  int indiceX = ligne.indexOf("X");
                  int indiceY = ligne.indexOf("Y");
                  int indiceZ = ligne.indexOf("Z");
                  float X = 0;float Y = 0;float Z = 0;

                  if (indiceX != -1){
                    X = ligne.mid(indiceX+1,8).simplified().toFloat();
                    //qDebug() << "X = " << ligne.mid(indiceX+1,8).simplified();
                    }
                  if (indiceY != -1){
                    Y = ligne.mid(indiceY+1,8).simplified().toFloat();
                    //qDebug() << "Y = "<< ligne.mid(indiceY+1,8).simplified();
                    }
                  if (indiceZ != -1){
                    Z = ligne.mid(indiceZ+1,8).simplified().toFloat();
                    //qDebug() << "Z = "<< ligne.mid(indiceZ+1,8).simplified();
                    }
                  //qDebug() << QString::number(X) << QString::number(Y) << QString::number(Z) << endl;
                  G01 * g01 = new G01(X,Y,Z,F);
                  liste.append(g01);
              }


              if (ligne.contains("F")){

                  F = ligne.mid(1,5).simplified().toFloat();
              }

              if (ligne.contains("G2")){
                  int indiceX = ligne.indexOf("X");
                  int indiceY = ligne.indexOf("Y");
                  int indiceI = ligne.indexOf("I");
                  int indiceJ = ligne.indexOf("J");
                  float X = 0;float Y = 0;float I = 0;float J = 0;

                  if (indiceX != -1){
                    X = ligne.mid(indiceX+1,8).simplified().toFloat();
                    //qDebug() << "X = " << ligne.mid(indiceX+1,8).simplified();
                    }
                  if (indiceY != -1){
                    Y = ligne.mid(indiceY+1,8).simplified().toFloat();
                    //qDebug() << "Y = "<< ligne.mid(indiceY+1,8).simplified();
                    }
                  if (indiceI != -1){
                    I = ligne.mid(indiceI+1,8).simplified().toFloat();
                    //qDebug() << "I = "<< ligne.mid(indiceI+1,8).simplified();
                    }
                  if (indiceJ != -1){
                    J = ligne.mid(indiceJ+1,8).simplified().toFloat();
                    //qDebug() << "J = "<< ligne.mid(indiceJ+1,8).simplified();
                    }
                  //qDebug() << QString::number(X) << QString::number(Y) << QString::numberI) << QString::numberJ) << endl;
                  G02 * g02 = new G02(X,Y,I,J,F);
                  liste.append(g02);
              }

              if (ligne.contains("SHAPE")){
                Figure * figure = new Figure();
                liste.append(figure);
              }

              if (ligne.contains("OUTPUT")){
                  QStringList Param = ligne.split(" ");
                  output *OutPut = new output(Param[1], Param[2]);
                  liste.append(OutPut);
              }

           }
    fichier_gcode.close();
    return liste;

}

QString Parser::type_check(Ligne *elt){

    if (dynamic_cast<Deplacement *>(elt)){return "Deplacement";}
    if (dynamic_cast<Figure *>(elt)){return "Figure";}
   else return "inconnu";
}


void Parser::write_liste(QList<Ligne *> liste, QString name_out){
     QString filename_out = QCoreApplication::applicationDirPath() + "/" + name_out;
     QFile out(filename_out);
     QTextStream stream_out(&out);
     out.open(QIODevice::WriteOnly | QIODevice::Text);

     for (int i = 0;i<liste.size();i++){
         stream_out << liste[i]->gcode_ligne() << endl;
     }
     out.close();
}


void Parser::insert_macro(QList<Ligne *> liste_abs, QList<Ligne *> macro, float distance_min, float distance_max){

    float total_distance = 0.,check_dist = 0.;
    QList<Ligne *> liste_sortie;
    int indice;
    for (int i = 0;i < liste_abs.size();i++){


            liste_sortie.append(liste_abs[i]);
            if (Parser::type_check(liste_abs[i]) == "Deplacement"){
                //qDebug() << QString::number(total_distance);
                total_distance += dynamic_cast<Deplacement *>(liste_abs[i])->get_distance();
                check_dist += dynamic_cast<Deplacement *>(liste_abs[i])->get_distance();
            }

            if (Parser::type_check(liste_abs[i]) == "Figure"){indice = i;}

            if(total_distance > distance_min){

                if ( dynamic_cast<Figure *>(liste_abs[indice])->get_taille() > distance_max ){
                    for (int j = 0;j < macro.size();j++){
                        liste_sortie.append(macro[j]);
                        }
                    total_distance = 0.;
                }

                else if (Parser::type_check(liste_abs[i]) == "Figure"){
                total_distance = 0.;
                //qDebug() << "macro in";
                for (int j = 0;j < macro.size();j++){
                    liste_sortie.append(macro[j]);
                    }
                }
            }
    }
    if(total_distance > distance_min){
        for (int j = 0;j < macro.size();j++){
            liste_sortie.append(macro[j]);
        }
    }

    write_liste(liste_sortie,"out.nc");
    //qDebug() << QString::number(check_dist);
}

void Parser::absolute_relative(QList<Ligne *> liste_gcode){

    QList<Ligne*> liste_dep;
    for(int i = 0;i<liste_gcode.size();i++){
        if (Parser::type_check(liste_gcode[i]) == "Deplacement"){
            liste_dep.append(liste_gcode[i]);
        }
    }

    float X_new,Y_new,X_actu,Y_actu,X_prec,Y_prec;
    QList<float> liste_new;

        liste_new.append(dynamic_cast<Deplacement *>(liste_dep[0])->get_info()[0]);
        liste_new.append(dynamic_cast<Deplacement *>(liste_dep[0])->get_info()[1]);

        for (int i = 1;i< liste_dep.size();i++){

            X_actu = dynamic_cast<Deplacement *>(liste_dep[i])->get_info()[0];
            Y_actu = dynamic_cast<Deplacement *>(liste_dep[i])->get_info()[1];

            X_prec = dynamic_cast<Deplacement *>(liste_dep[i-1])->get_info()[0];
            Y_prec = dynamic_cast<Deplacement *>(liste_dep[i-1])->get_info()[1];

            X_new = X_actu - X_prec;
            Y_new = Y_actu - Y_prec;

            if (X_actu == 0 && Y_actu == 0){
                dynamic_cast<Deplacement *>(liste_dep[i])->set_info_abs(X_prec,Y_prec);
                X_new = 0;
                Y_new = 0;
            }
            liste_new.append(X_new);
            liste_new.append(Y_new);
        }

        float X_rel,Y_rel;
        for (int i = 0;i< liste_dep.size();i++){
            X_rel = liste_new.takeFirst();
            Y_rel = liste_new.takeFirst();
            dynamic_cast<Deplacement *>(liste_dep[i])->set_info_rel(X_rel,Y_rel);
        }

        for(int i = 0;i<liste_gcode.size();i++){
                    if (Parser::type_check(liste_gcode[i]) == "Deplacement"){
                        liste_gcode.replace(i,liste_dep.takeFirst());
                    }
        }

    compute_taille_figure(liste_gcode);
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

void Parser::correspondance(QList<Ligne *> liste_abs, QString name){

    QString filename_corres = QCoreApplication::applicationDirPath() + "/" + name;
    QFile fichier_corres(filename_corres);
    QTextStream fichier_in(&fichier_corres);

    fichier_corres.open(QIODevice::ReadOnly | QIODevice::Text);
    QString ligne;

    while (!fichier_in.atEnd()){
        ligne = fichier_in.readLine();

        if(ligne.contains("Distance")){
            QStringList liste = ligne.split(" ");
            insert_macro(liste_abs,Parser::parse_gcode(liste[4]),liste[2].toFloat(),liste[3].toFloat());

        }

    }

}





