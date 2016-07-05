#include "input.h"

input::input(QString pin, QString val): _pin(pin), _val(val){};


QString input::gcode_ligne(){

    //QDebug() << "OUTPUT " + _pin + " " + _val;
    return  "INPUT " + _pin + " " + _val;

}
