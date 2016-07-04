#include "output.h"

output::output(QString relais, QString val): _relais(relais), _val(val){}


QString output::gcode_ligne(){

    //QDebug() << "OUTPUT " + _relais + " " + _val;
    return  "OUTPUT " + _relais + " " + _val;

}
