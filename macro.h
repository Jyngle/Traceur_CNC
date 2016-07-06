#ifndef MACRO_H
#define MACRO_H

#include <QString>
#include <QListIterator>
#include <QDebug>

#include "ligne.h"
#include "g00.h"

class macro : public Ligne
{
public:
    QList<Ligne *> _ListeGcodeMacro;
    macro(QString name, QList<Ligne *> ListeGcodeMacro);

    void SetPositionRetour(float X, float Y, float Z);
    virtual QString gcode_ligne();

    QString _name;



};

#endif // MACRO_H
