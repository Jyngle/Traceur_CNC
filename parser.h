#ifndef PARSER_H
#define PARSER_H

#include <QList>
#include "ligne.h"
#include "output.h"
#include "pause.h"
#include "input.h"
#include "macro.h"
#include "config.h"
#include "g01.h"




class Parser
{

public:

    void ReadInputFile();
    void WriteOutputFile();
    void AjoutMacros();
    void check_depacement();
    int ComputeTime();
    void compute_arc();

protected:
    struct Position{
        float X;
        float Y;
        float Z;
        int F;
    };

    

    
    QList<Ligne *> _ListeGcode;

    void parse_gcode_file(QString name, QList<Ligne *> &__ListeGcode, float X, float Y, float Z, int F);

    void insert_macro_distance(QString FileNameMacro, float distance_min, float distance_max);

    void insert_macro_debut(QString FileNameMacro);

    void insert_macro_fin(QString FileNameMacro);

    void insert_macro_at(QString name,QString FileNameMacro, int Index);

    void read_ardware_limitFile(Position& abs);

    int read_param_file(QString key, QString & value);

    void absolute_relative();
    void compute_taille_figure(QList<Ligne *> liste_gcode);
    void clean_file();
    int GetValue(QString ligne, QString Key, QString &Value);
    void scanDeltaZ(QString inputFile);
    QString type_check(Ligne *elt);

};

#endif // PARSER_H
