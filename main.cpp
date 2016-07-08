#include "parser.h"
#include "deplacement.h"
#include "figure.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "Parser Gcode pour RaspberryPi" << endl;


    Parser GParser;

    try
    {
        GParser.ReadInputFile("source.nc");
        GParser.AjoutMacros("correspondance.txt");
        GParser.WriteOutputFile("out.nc");
        GParser.check_depacement();
    }
    catch(QString const& chaine)
    {
        qDebug() << chaine << endl;
        return 1;
    }


    return 0;
}
