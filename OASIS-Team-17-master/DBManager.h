#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVector>
#include <QDebug>

#include "Session.h"
#include "Preferences.h"

/* Purpose: The purpose of this function is to facilitate the storage of a given user's preferences and session history for later access
 *
 * Data Members:
 * - sessionDB is the QSqlDatabase object that will be the created database used in this function
 * + DATABASE_PATH is a qstring which stores the path to which the database will be stored
 *
 * Functions:
 * - initDB() initializes the database but starting a transaction and creating the required tables
 * + clearDB() allows for the deletion of all the values and table in case of machine reset.
 * + addSession(QString&, int, int) takes in the parameters to create a new session object
 * + addPreferences(QString, int) takes in the parameters of a preferences object, it follows by inserting each of the parameters in their appropriate order into the DB and returns true if it has successfullly commited the changes
 * + getSessions() this function returns a qvector of session*, this function is a getter for all the sessions that exist within the DB which are queried, turned into session objects and pushed to the vector.
 * + getPreferences*(QString) this function returns a preferences*, this function is a getter for the preferences at a certain type from the DB upon quering is turned into an object.
 */

class DBManager{
public:
    DBManager();
    bool clearDB();
    bool addSession(QString&, int, int);
    bool addPreference(QString,int);
    QVector<Session*> getSessions();
    Preferences* getPreference(QString);
    static const QString DATABASE_PATH;


private:
    QSqlDatabase sessionDB;
    bool initDB();

};

#endif // DBMANAGER_H
