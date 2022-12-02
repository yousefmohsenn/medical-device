#ifndef SESSION_H
#define SESSION_H

#include <QString>

/* Purpose: The purpose of this function is to create a session class which will allow for the storage of a session type, pulse type, duration and intensity
 *
 * Data Members:
 * - pulseType is a QString that stores the pulse type
 * - sessionType is a QString that stores the session type
 * - intensity is an int that stores the integer value of the intensity
 * - duration is an int that stores the integer value of the duration
 *
 * Functions:
 * - setIntensity(int) is a function that takes in an int that will be used to set the intensity of this object
 * - getDuration() returns a int of the duration stored in this object
 * - getType() returns a Qstring of the type stored in this object
 * - getIntensity() returns a int of the intensity stored in this object
*/

class Session{
public:
    Session(int, QString, QString);
    Session(int, QString, int);
    ~Session();
    void setIntensity(int);
    int getIntensity();
    int getDuration();
    QString getType();

private:
    int intensity;
    int duration;
    QString pulseType;
    QString sessionType;

};

#endif // SESSION_H
