#include "Session.h"


Session::Session(int d, QString pt, QString s)
    :duration(d), pulseType(pt), sessionType(s)
{
    intensity = 1;
}

Session::Session(int d, QString s, int i)
    :duration(d), sessionType(s), intensity(i)
{}

Session::~Session(){}

//set intensity
void Session::setIntensity(int i)
{intensity += i;}

//return intensity
int Session::getIntensity()
{return intensity;}

//return duration
int Session::getDuration()
{return duration;}

//return type
QString Session::getType()
{return sessionType;}
