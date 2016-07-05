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

    Parser::AjoutMacros(liste_gcode,"correspondance.txt");

    return 0;
}
