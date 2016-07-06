#ifndef PARSER_H
#define PARSER_H

#include <QList>
#include "ligne.h"
#include "output.h"
#include "pause.h"
#include "input.h"


class Parser
{

public:

    void ReadInputFile(QString FileNameIN);
    void WriteOutputFile(QString FileNameOUT);
    void AjoutMacros(QString FileNameCorespondance);


protected:


    QList<Ligne *> _ListeGcode;

    void parse_gcode_file(QString name, QList<Ligne *> &__ListeGcode, float X, float Y, float Z, int F);

    void insert_macro_distance(QList<Ligne *> liste_abs, QList<Ligne *> macro, float distance_min, float distance_max);
    //static void insert_macro_distance2(QList<Ligne *> &liste_entre, QList<Ligne *> macro, float distance_min, float distance_max);

    void absolute_relative();
    void compute_taille_figure(QList<Ligne *> liste_gcode);
    void clean_file(QString name);
    int GetValue(QString ligne, QString Key, QString &Value);
    QString type_check(Ligne *elt);

};

#endif // PARSER_H
