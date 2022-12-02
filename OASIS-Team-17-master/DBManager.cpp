#include <DBManager.h>
const QString DBManager::DATABASE_PATH = "/database/session.db";

DBManager::DBManager(){
    //Initially create the tables and ensure no issues.
    sessionDB = QSqlDatabase::addDatabase("QSQLITE");
    sessionDB.setDatabaseName("session.db");

    if(sessionDB.open()){
        qInfo("DB successful!");
    }else{
        throw "ERROR: Unable to open DB";
    }

    if(!initDB()){
        throw "ERROR: Unable to init DB";
    }
}

bool DBManager::initDB(){
    //Create initiale tables
    sessionDB.transaction();
    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS sessions ( type TEXT NOT NULL, duration INTEGER NOT NULL, level INTEGER NOT NULL);");
    q.exec("CREATE TABLE IF NOT EXISTS preferences ( mode TEXT NOT NULL, hz INTEGER NOT NULL);");

    return sessionDB.commit();
}

bool DBManager::clearDB(){
    //Delete values from tables followed by dropping tables
    sessionDB.transaction();
    QSqlQuery q;
    q.prepare("DELETE FROM sessions");
    q.exec();
    q.prepare("DELETE FROM preferences");
    q.exec();

    return sessionDB.commit();
}

bool DBManager::addSession(QString& st, int dur, int intes){
    //Create query that inserts parameters to DB
    sessionDB.transaction();
    QSqlQuery q;
    q.prepare("INSERT INTO sessions (type, duration, level) VALUES (:type, :duration, :level);");
    q.bindValue(":type", st);
    q.bindValue(":duration", dur);
    q.bindValue(":level", intes);
    q.exec();

    return sessionDB.commit();
}

bool DBManager::addPreference(QString mode, int hz){
    //Create query that inserts parameters to DB
    sessionDB.transaction();
    QSqlQuery q;
    q.prepare("INSERT INTO preferences (mode, hz) VALUES (:mode, :hz);");
    q.bindValue(":mode", mode);
    q.bindValue(":hz", hz);
    q.exec();

    return sessionDB.commit();
}

QVector<Session*> DBManager::getSessions(){
    //Create QVector of session*
    //Query all sessions from table, while results exist create session object and append to vector. Return QVector when finished
    QVector<Session*> sess;
    QSqlQuery q;
    q.exec("SELECT * FROM sessions");
    while(q.next()){
        QString sessionType = q.value(0).toString();
        int duration = q.value(1).toInt();
        int level = q.value(2).toInt();
        Session* ns = new Session(duration, sessionType, level);
        sess.append(ns);
    }
    return sess;
}

Preferences* DBManager::getPreference(QString type){
    //Query preferences for QString type, if null return defualt "null" and 1 if not return the obtained preference from DB.
    sessionDB.transaction();
    Preferences* pref;
    QSqlQuery q;
    q.prepare("SELECT * FROM preferences WHERE mode=:mode");
    q.bindValue(":mode",type);
    q.exec();

    if(!q.next()){
        pref = new Preferences("null", 1);
        return pref;
    }

    QString mode = q.value(0).toString();
    int hz = q.value(1).toInt();
    pref = new Preferences(mode, hz);

    return pref;
}


