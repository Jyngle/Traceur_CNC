#include "pause.h"

pause::pause(QString delay): _delay(delay){}

QString pause::gcode_ligne(){

    //QDebug() << "PAUSE " + _delay;
    return  "PAUSE " + _delay;

}
