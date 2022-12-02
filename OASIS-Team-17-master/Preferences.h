#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "QString"

/* Purpose: The purpose of this function is to create a preference class which will allow for the storage of a session type and intensity
 *
 * Data Members:
 * - type is a QString that stores the session type
 * - intensity is an int that stores the integer value of the intensity
 *
 * Functions:
 * - getType() returns a Qstring of the type stored in this object
 * - getIntensity() returns a int of the intensity stored in this object
*/

class Preferences{
public:
    Preferences(QString, int);
    QString getType();
    int getIntensity();

private:
    QString type;
    int intensity;

};

#endif // PREFERENCES_H
