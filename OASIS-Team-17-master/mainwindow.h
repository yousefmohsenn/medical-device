#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include "Session.h"
#include "Preferences.h"
#include "DBManager.h"

/* Purpose: the main window of the UI, including all UI functionality
 *
 * Data Members:
 *- ui is the ui object
 *- power the device power state
 *- lowBattery the device has low battery state
 *- battery the numerical representation of the battery
 *- selectionSession the current session's duration type
 *- selectedType the current session's type
 *- currentSession the current session object
 *- sessionRunning if a session is running or not
 *- testRunning if a connection test is running or not
 *- selectedPulse the pulse type of the session
 *- connectionStrength how strong the connection is
 *- userDesTime the user's designated duration
 *- batteryShowing the battery graph is currently visible
 *- endedEarly the session ended early
 *- timers QVector to store the locallly made timers in the session
 *- nonMenuState not in the menu before starting a session
 *- disruptCheck the session is being disrupted
 *- sessionHistory store the stored sessions
 *- typePreferences store the preferences for sessions
 *
 * Functions:
 *+ connectSetup() setup the UI
 *+ lowBatteryCheck() check for low battery
 *+ softOn(int) slowly turn the intensity up on session based on preference
 *
 *- getSessionHistory() get the sessions from the database
 *- saveSession() save a session
 *- dropDatabase() drop the database
 *- getPreferences() get preferences from database
 *- swapGraph() switch between visible graphs
 *- clearBattery() clear the battery graph
 *- clearIntensity() clear the intensity graph
 *- lowBatteryBlink() blink the battery
 *- displayBattery() display the battery level
 *- depleteBattery() decerease battery level
 *- powerPressed() detect power button press
 *- powerHold() detect power button held down
 *- upPressed() increase intensity or change type
 *- downPressed() decrease intensity or change type
 *- selectPressed() select button pressed
 *- userLPressed() user intensity decrease time
 *- userRPressed() user intensity increase time
 *- powerOff() turn off power
 *- updateBattery() update the battery level admin panel
 *- updateConnection() update the connection admin panel
 *- updateDisrupt() disrupt connection during session
 *- disruptRestore() restore the session after disrupting it
 *- blinkElement() blink a UI element
 *- unblinkElement() revert blinked UI element
 *- connectionTest() run the connection test
 *- noConnection() blink the bad connection on graph
 *- graphAnimation() start a graph animation
 *- animationSwitch() switch between steps on the graph
 *- softOff() slowly decrease intensity until session ends
 *- softOffAnimation() animate the soft off
 *- softOnAnimation() animate the soft on
 *- beginSession() begin the session
 *- endSession() end the session
 */

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void connectSetup();
    void lowBatteryCheck();
    void softOn(int);



private:

    Ui::MainWindow *ui;
    DBManager* db;

    QTimer* clickTime;
    bool power;
    bool lowBattery;
    double battery;
    QString selectedSession;
    QString selectedType;
    Session* currentSession;
    bool sessionRunning;
    bool testRunning;
    QString selectedPulse;
    QString connectionStrength;
    int userDesTime;
    bool userDesSelected;
    bool batteryShowing;
    bool endedEarly;
    QVector<QTimer*> timers;
    bool nonMenuState;
    bool disruptCheck;

    QVector<Session*> sessionHistory;
    QVector<Preferences*> typePreferences;




private slots:
    void getSessionHistory();
    void saveSession();
    void dropDatabase();
    void getPreferences();

    void swapGraph();

    void clearBattery();
    void clearIntensity();

    void lowBatteryBlink();
    void displayBattery();
    void depleteBattery();

    void powerPressed();
    void powerHold();
    void upPressed();
    void downPressed();
    void selectPressed();
    void userLPressed();
    void userRPressed();

    void powerOff();

    void updateBattery();
    void updateConnection();

    void updateDisrupt();
    void disruptRestore();

    void blinkElement();
    void unblinkElement();

    void connectionTest();
    void noConnection();

    void graphAnimation();
    void animationSwitch();

    void softOff();
    void softOffAnimation();


    void softOnAnimation();

    void beginSession();
    void endSession();

};
#endif // MAINWINDOW_H
