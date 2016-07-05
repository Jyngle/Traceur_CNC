#include "parser.h"
#include "deplacement.h"
#include "figure.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "Parser Gcode pour RaspberryPi" << endl;

    Parser::clean_file("source.nc");

    QList<Ligne*> liste_gcode = Parser::parse_gcode("gcode.nc");

    Parser::absolute_relative(liste_gcode);


    /*for (int i = 0;i<liste_gcode.size();i++){
        if (Parser::type_check(liste_gcode[i]) == "Deplacement"){
            qDebug() << QString::number(dynamic_cast<Deplacement *>(liste_gcode[i])->get_info_rel()[0]);
            qDebug() << QString::number(dynamic_cast<Deplacement *>(liste_gcode[i])->get_info_rel()[1]);
        }
    }*/

    Parser::AjoutMacros(liste_gcode,"correspondance.txt");

    return 0;
}
