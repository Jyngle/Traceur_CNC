#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QTime>
#include "parser.h"
#include "deplacement.h"
#include "figure.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "Parser Gcode pour RaspberryPi" << endl;


    Parser GParser;

    try
    {
        GParser.ReadInputFile("source.nc");
        GParser.AjoutMacros("correspondance.txt");
        GParser.check_depacement();
    }
    catch(QString const& chaine)
    {
        QFile errFile(QCoreApplication::applicationDirPath() + "/" + "Err.txt");
        QTextStream stream_err(&errFile);
        errFile.open(QIODevice::WriteOnly | QIODevice::Text);

        unsigned int heures = QTime::currentTime().hour();
        unsigned int minutes = QTime::currentTime().minute();
        QDate date(QDate::currentDate());

        stream_err << "Erreur genere le " << QString::number(date.day()) << "/" << QString::number(date.month()) << "/" << QString::number(date.year());
        stream_err << " a " << QString::number(heures) << "H" << QString::number(minutes) << "   -->" << endl;

        stream_err << chaine << endl;
        errFile.close();

        qDebug() << chaine << endl;
        return 1;
    }

    GParser.WriteOutputFile("out.nc");

    return 0;
}
