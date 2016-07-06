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

    static QList<Ligne *> parse_gcode(QString name);

    static QString type_check(Ligne *elt);

    static void insert_macro_distance(QList<Ligne *> liste_abs, QList<Ligne *> macro, float distance_min, float distance_max);
    //static void insert_macro_distance2(QList<Ligne *> &liste_entre, QList<Ligne *> macro, float distance_min, float distance_max);
    static void write_liste(QList<Ligne *> liste, QString name_out);
    static void absolute_relative(QList<Ligne *> liste_gcode);
    static void compute_taille_figure(QList<Ligne *> liste_gcode);
    static void clean_file(QString name);
    static void AjoutMacros(QList<Ligne *> liste_abs, QString name);
    static int GetValue(QString ligne, QString Key, QString &Value);


};

#endif // PARSER_H
