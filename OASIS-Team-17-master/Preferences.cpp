#include "Preferences.h"

Preferences:: Preferences(QString m, int i){
    type = m;
    intensity = i;
}

//return type
QString Preferences:: getType()
{return type;}

//return intensity
int Preferences:: getIntensity()
{return intensity;}



