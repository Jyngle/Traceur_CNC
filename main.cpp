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

    QFile errFile(QCoreApplication::applicationDirPath() + ERROR);
    QTextStream stream_log(&errFile);
    errFile.open(QIODevice::WriteOnly | QIODevice::Text);

    Parser GParser;

    try
    {
        GParser.ReadInputFile();
        GParser.AjoutMacros();
        GParser.check_depacement();
        stream_log << "Temps aproximatif du dessin : " << QString::number(GParser.ComputeTime()) << "min" << endl;
    }
    catch(QString const& chaine)
    {


        unsigned int heures = QTime::currentTime().hour();
        unsigned int minutes = QTime::currentTime().minute();
        QDate date(QDate::currentDate());

        stream_log << "Erreur genere le " << QString::number(date.day()) << "/" << QString::number(date.month()) << "/" << QString::number(date.year());
        stream_log << " a " << QString::number(heures) << "H" << QString::number(minutes) << "   -->" << endl;

        stream_log << chaine << endl;
        errFile.close();

        qDebug() << chaine << endl;
        return 1;
    }


    GParser.WriteOutputFile();


    errFile.close();
    return 0;
}
