#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Session.h"
#include <QtMath>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Initialize variables
    power = false;
    sessionRunning = false;
    nonMenuState = false;
    testRunning = false;
    disruptCheck = false;
    battery = 100;
    lowBattery = false;
    selectedSession = "20";
    selectedType = "alpha";
    selectedPulse = "spulse";
    connectionStrength = "strong";
    userDesTime = 20;
    userDesSelected = false;
    batteryShowing = false;
    ui->setupUi(this);

    connectSetup();
}

MainWindow::~MainWindow()
{delete ui;}

void MainWindow:: connectSetup(){
    //Setting up all of the ui and database

    db = new DBManager();

    getSessionHistory();
    Preferences* alphaP = new Preferences("alpha", 1);
    Preferences* beta1P = new Preferences("beta1", 1);
    Preferences* beta2P = new Preferences("beta2", 1);
    Preferences* hzP = new Preferences("hz", 1);

    typePreferences.append(alphaP);
    typePreferences.append(beta1P);
    typePreferences.append(beta2P);
    typePreferences.append(hzP);

    getPreferences();

    ui->one->setVisible(true);
    ui->two->setVisible(true);
    ui->three->setVisible(true);
    ui->four->setVisible(true);
    ui->five->setVisible(true);
    ui->six->setVisible(true);
    ui->seven->setVisible(true);
    ui->eight->setVisible(true);

    ui->one_2->setVisible(false);
    ui->two_2->setVisible(false);
    ui->three_2->setVisible(false);
    ui->four_2->setVisible(false);
    ui->five_2->setVisible(false);
    ui->six_2->setVisible(false);
    ui->seven_2->setVisible(false);
    ui->eight_2->setVisible(false);

    clickTime = new QTimer(this);
    clickTime->setSingleShot(true);
    clickTime->setInterval(2001);

    connect(ui->powerButton, SIGNAL(pressed()), this, SLOT (powerPressed()));
    connect(ui->powerButton, SIGNAL(released()), this, SLOT (powerHold()));

    connect(ui->intUpButton, SIGNAL(released()), this, SLOT (upPressed()));
    connect(ui->intDownButton, SIGNAL(released()), this, SLOT (downPressed()));
    connect(ui->selectButton, SIGNAL(released()), this, SLOT (selectPressed()));

    connect(ui->userTimeL, SIGNAL(released()), this, SLOT (userLPressed()));
    connect(ui->userTimeR, SIGNAL(released()), this, SLOT (userRPressed()));

    connect(ui->batteryBox, SIGNAL(valueChanged(int)), this, SLOT (updateBattery()));
    connect(ui->strengthBox, SIGNAL(currentTextChanged(QString)), this, SLOT (updateConnection()));
    connect(ui->disruptButton, SIGNAL(released()), this, SLOT (updateDisrupt()));

    connect(ui->saveButton, SIGNAL(released()), this, SLOT (saveSession()));

    connect(ui->dropDB, SIGNAL(released()), this, SLOT(dropDatabase()));
}

void MainWindow:: getSessionHistory(){
    //Retrieve the sessions from the database

    sessionHistory.clear();
    ui->listWidget->clear();

    sessionHistory = db->getSessions();

    for(int i = 0; i < sessionHistory.size(); i++){
        QListWidgetItem* sess = new QListWidgetItem();

        QString pastSession = "Type: " + sessionHistory.at(i)->getType() + " | Duration: " + QString::number(sessionHistory.at(i)->getDuration()) + " | Intensity: " + QString::number(sessionHistory.at(i)->getIntensity());
        sess->setText(pastSession);
        ui->listWidget->addItem(sess);
    }
}

void MainWindow:: getPreferences(){
    //Retrieve the preferences from the database

    if(db->getPreference("alpha")->getType() != "null"){
        typePreferences.insert(typePreferences.begin() + 0, db->getPreference("alpha"));
    }
    if(db->getPreference("beta1")->getType() != "null"){
        typePreferences.insert(typePreferences.begin() + 1, db->getPreference("beta1"));
    }
    if(db->getPreference("beta2")->getType() != "null"){
        typePreferences.insert(typePreferences.begin() + 2, db->getPreference("beta2"));
    }
    if(db->getPreference("hz")->getType() != "null"){
        typePreferences.insert(typePreferences.begin() + 3, db->getPreference("hz"));
    }

}

void MainWindow:: dropDatabase(){
    //Drop the database
    db->clearDB();
    getSessionHistory();
}

void MainWindow:: swapGraph(){
    //Switch between the graph that is used for the battery and connection, to the graph that is used for intensity

    if(batteryShowing == true){
        ui->one_2->setVisible(false);
        ui->two_2->setVisible(false);
        ui->three_2->setVisible(false);
        ui->four_2->setVisible(false);
        ui->five_2->setVisible(false);
        ui->six_2->setVisible(false);
        ui->seven_2->setVisible(false);
        ui->eight_2->setVisible(false);

        ui->one->setVisible(true);
        ui->two->setVisible(true);
        ui->three->setVisible(true);
        ui->four->setVisible(true);
        ui->five->setVisible(true);
        ui->six->setVisible(true);
        ui->seven->setVisible(true);
        ui->eight->setVisible(true);

        batteryShowing = false;
    } else {
        ui->one->setVisible(false);
        ui->two->setVisible(false);
        ui->three->setVisible(false);
        ui->four->setVisible(false);
        ui->five->setVisible(false);
        ui->six->setVisible(false);
        ui->seven->setVisible(false);
        ui->eight->setVisible(false);

        ui->one_2->setVisible(true);
        ui->two_2->setVisible(true);
        ui->three_2->setVisible(true);
        ui->four_2->setVisible(true);
        ui->five_2->setVisible(true);
        ui->six_2->setVisible(true);
        ui->seven_2->setVisible(true);
        ui->eight_2->setVisible(true);

        batteryShowing = true;
    }
}

void MainWindow::displayBattery(){
    //Display the current battery level on the graph

    swapGraph();

    QTimer* count = new QTimer();
    count->setSingleShot(true);
    count->setInterval(2000);

    connect(count, SIGNAL(timeout()), this, SLOT(swapGraph()));

    clearBattery();

    count->start();
    if(battery <= 100 && battery > 50){
        ui->one_2->setStyleSheet("QLabel {background-color:green;}");
        ui->two_2->setStyleSheet("QLabel {background-color:green;}");
        ui->three_2->setStyleSheet("QLabel {background-color:green;}");
    }else if(battery <= 50 && battery > 25){
        ui->four_2->setStyleSheet("QLabel {background-color:yellow;}");
        ui->five_2->setStyleSheet("QLabel {background-color:yellow;}");
        ui->six_2->setStyleSheet("QLabel {background-color:yellow;}");
    }else{
        ui->seven_2->setStyleSheet("QLabel {background-color:red;}");
        ui->eight_2->setStyleSheet("QLabel {background-color:red;}");
    }
}

void MainWindow:: clearBattery(){
    //Clear the battery graph
    ui->one_2->setStyleSheet("QLabel {}");
    ui->two_2->setStyleSheet("QLabel {}");
    ui->three_2->setStyleSheet("QLabel {}");
    ui->four_2->setStyleSheet("QLabel {}");
    ui->five_2->setStyleSheet("QLabel {}");
    ui->six_2->setStyleSheet("QLabel {}");
    ui->seven_2->setStyleSheet("QLabel {}");
    ui->eight_2->setStyleSheet("QLabel {}");
}

void MainWindow:: clearIntensity(){
    //Clear the intensity graph
    ui->one->setStyleSheet("QLabel {}");
    ui->two->setStyleSheet("QLabel {}");
    ui->three->setStyleSheet("QLabel {}");
    ui->four->setStyleSheet("QLabel {}");
    ui->five->setStyleSheet("QLabel {}");
    ui->six->setStyleSheet("QLabel {}");
    ui->seven->setStyleSheet("QLabel {}");
    ui->eight->setStyleSheet("QLabel {}");
}

void MainWindow:: depleteBattery(){
    //Deplete the battery based on static presses or intensity

    if(sessionRunning == false){
        battery -= battery* 0.01;
    }else{
        if(currentSession->getIntensity() <= 2){
            battery -= battery*0.03;
        }else if(currentSession->getIntensity() <= 4){
            battery -= battery*0.06;
        }else if(currentSession->getIntensity() <= 6){
            battery -= battery*0.09;
        }else if(currentSession->getIntensity() <= 8){
            battery -= battery*0.12;
        }
    }
    lowBatteryCheck();
}

void MainWindow:: lowBatteryCheck(){
    //Check if battery is below certain thresholds and animate

    if(battery <= 10){
        clearBattery();
        if(sessionRunning == true){
            for(int i = 0; i < timers.size(); i++){
                timers.at(i)->stop();
            }
            timers.clear();
            endSession();
            sessionRunning = false;
        }
        ui->eight_2->setStyleSheet("QLabel {background-color:red;}");
        int interval = 250;
        for(int i = 0; i < 8; i++){
            QTimer* batteryTimer = new QTimer();
            batteryTimer->setSingleShot(true);
            batteryTimer->setInterval(interval);
            interval += 250;
            connect(batteryTimer, SIGNAL(timeout()), this, SLOT (lowBatteryBlink()));
            batteryTimer->start();
        }
        QTimer* offTimer = new QTimer();
        offTimer->setSingleShot(true);
        offTimer->setInterval(2100);
        connect(offTimer, SIGNAL(timeout()), this, SLOT (powerOff()));
        offTimer->start();
    }else if(battery <= 30){
        clearBattery();
        ui->seven_2->setStyleSheet("QLabel {background-color:yellow;}");
        ui->eight_2->setStyleSheet("QLabel {background-color:yellow;}");
        int interval = 250;
        for(int i = 0; i < 8; i++){
            QTimer* batteryTimer = new QTimer();
            batteryTimer->setSingleShot(true);
            batteryTimer->setInterval(interval);
            interval += 250;
            connect(batteryTimer, SIGNAL(timeout()), this, SLOT (lowBatteryBlink()));
            batteryTimer->start();
        }
        QTimer* restoreTimer = new QTimer();
        restoreTimer->setSingleShot(true);
        restoreTimer->setInterval(2500);
        connect(restoreTimer, SIGNAL(timeout()), this, SLOT(swapGraph()));
        restoreTimer->start();
    }
}

void MainWindow:: lowBatteryBlink(){
    //Blink the battery between off and on

    if(battery <=10){
        if(ui->eight_2->styleSheet() == "QLabel {background-color:red}"){
            ui->eight_2->setStyleSheet("QLabel {}");
        }else{
            ui->eight_2->setStyleSheet("QLabel {background-color:red}");
        }
    }else{
        if(ui->eight_2->styleSheet() == "QLabel {background-color:yellow;}"){
            ui->eight_2->setStyleSheet("QLabel {}");
            ui->seven_2->setStyleSheet("QLabel {}");

        }else{
            ui->eight_2->setStyleSheet("QLabel {background-color:yellow;}");
            ui->seven_2->setStyleSheet("QLabel {background-color:yellow;}");
        }
    }
}

void MainWindow:: powerPressed(){
    //Start timer for detecting how long button was pressed for
    clickTime->start();
}

void MainWindow:: powerHold(){
    //Check if the button was short or long pressed (2 seconds)

    if(clickTime->remainingTime() > 0){
        if(power == false){
            return;
        }
        if(nonMenuState == true){
            return;
        }
        if(sessionRunning == true){
            softOff();
        } else {
            depleteBattery();
            //Selecting a session with the power button
            if(selectedSession == "20"){
                selectedSession = "45";
                ui->twenty->setStyleSheet("QPushButton {}");
                ui->forty->setStyleSheet("QPushButton {background-color:yellow;}");
            }else if(selectedSession == "45"){
                selectedSession = "User";
                ui->forty->setStyleSheet("QPushButton {}");
                ui->user->setStyleSheet("QPushButton {background-color:yellow;}");
                userDesSelected = true;
                ui->userTimeL->setEnabled(true);
                ui->userTimeR->setEnabled(true);
            }else if(selectedSession == "User"){
                selectedSession = "20";
                ui->user->setStyleSheet("QPushButton {}");
                ui->twenty->setStyleSheet("QPushButton {background-color:yellow;}");
                userDesSelected = false;
                ui->userTimeL->setEnabled(false);
                ui->userTimeR->setEnabled(false);
            }
        }
    }else{
        if(power == false){
            //Turning the device on

            ui->one->setStyleSheet("QLabel {background-color:yellow;}");
            ui->twenty->setStyleSheet("QPushButton {background-color:yellow;}");
            ui->LED_signal->setStyleSheet("QPushButton {background-color:green;}");
            ui->alpha->setStyleSheet("QPushButton {background-color:yellow;}");
            ui->spulse->setStyleSheet("QPushButton {background-color:yellow;}");

            ui->selectButton->setEnabled(true);
            ui->intUpButton->setEnabled(true);
            ui->intDownButton->setEnabled(true);

            power = true;
            displayBattery();
            lowBatteryCheck();
        }else{
            powerOff();
        }
    }
    clickTime->stop();
}

void MainWindow:: powerOff(){
    //Turn the device off, disable certain elements

    ui->twenty->setStyleSheet("QPushButton {}");
    ui->forty->setStyleSheet("QPushButton {}");
    ui->user->setStyleSheet("QPushButton {}");
    ui->LED_signal->setStyleSheet("QPushButton {}");
    ui->spulse->setStyleSheet("QPushButton {}");

    ui->alpha->setStyleSheet("QPushButton {}");
    ui->beta1->setStyleSheet("QPushButton {}");
    ui->beta2->setStyleSheet("QPushButton {}");
    ui->hz->setStyleSheet("QPushButton {}");

    ui->selectButton->setEnabled(false);
    ui->intUpButton->setEnabled(false);
    ui->intDownButton->setEnabled(false);

    selectedSession = "20";
    selectedType = "alpha";
    selectedPulse = "spulse";
    userDesTime = 20;

    sessionRunning = false;
    nonMenuState = false;
    testRunning = false;

    userDesSelected = false;

    clearBattery();
    clearIntensity();

    power = false;
}

void MainWindow:: upPressed(){
    //Change session type or intensity up

    if(sessionRunning == false){
        depleteBattery();
        if(selectedType == "alpha"){
            selectedType = "beta1";
            selectedPulse = "spulse";
            ui->alpha->setStyleSheet("QPushButton {}");
            ui->one->setStyleSheet("QLabel {}");
            ui->beta1->setStyleSheet("QPushButton {background-color:yellow;}");
            ui->two->setStyleSheet("QLabel {background-color:yellow;}");
            ui->spulse->setStyleSheet("QPushButton {background-color:yellow;}");

        }else if(selectedType == "beta1"){
            selectedType = "beta2";
            selectedPulse = "spulse";
            ui->beta1->setStyleSheet("QPushButton {}");
            ui->two->setStyleSheet("QLabel {}");
            ui->beta2->setStyleSheet("QPushButton {background-color:yellow;}");
            ui->three->setStyleSheet("QLabel {background-color:yellow;}");
            ui->spulse->setStyleSheet("QPushButton {background-color:yellow;}");

        }else if(selectedType == "beta2"){
            selectedType = "hz";
            selectedPulse = "spulse";
            ui->beta2->setStyleSheet("QPushButton {}");
            ui->three->setStyleSheet("QLabel {}");
            ui->hz->setStyleSheet("QPushButton {background-color:yellow;}");
            ui->four->setStyleSheet("QLabel {background-color:yellow;}");
            ui->spulse->setStyleSheet("QPushButton {background-color:yellow;}");
        }else if(selectedType == "hz"){
            selectedType = "alpha";
            selectedPulse = "spulse";
            ui->hz->setStyleSheet("QPushButton {}");
            ui->four->setStyleSheet("QLabel {}");
            ui->alpha->setStyleSheet("QPushButton {background-color:yellow;}");
            ui->one->setStyleSheet("QLabel {background-color:yellow;}");
            ui->spulse->setStyleSheet("QPushButton {background-color:yellow;}");
        }
    } else {
        if(currentSession->getIntensity() == 8){
            currentSession->setIntensity(-7);

            ui->eight->setStyleSheet("QLabel {}");
            ui->one->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 7){
            currentSession->setIntensity(1);

            ui->seven->setStyleSheet("QLabel {}");
            ui->eight->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 6){
            currentSession->setIntensity(1);

            ui->six->setStyleSheet("QLabel {}");
            ui->seven->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 5){
            currentSession->setIntensity(1);

            ui->five->setStyleSheet("QLabel {}");
            ui->six->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 4){
            currentSession->setIntensity(1);

            ui->four->setStyleSheet("QLabel {}");
            ui->five->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 3){
            currentSession->setIntensity(1);

            ui->three->setStyleSheet("QLabel {}");
            ui->four->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 2){
            currentSession->setIntensity(1);

            ui->two->setStyleSheet("QLabel {}");
            ui->three->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 1){
            currentSession->setIntensity(1);

            ui->one->setStyleSheet("QLabel {}");
            ui->two->setStyleSheet("QLabel {background-color:yellow;}");
        }
    }
}

void MainWindow:: downPressed(){
    //Change session type or intensity down

    if(sessionRunning == false){
        depleteBattery();
        if(selectedType == "alpha"){
            selectedType = "hz";
            selectedPulse = "spulse";
            ui->alpha->setStyleSheet("QPushButton {}");
            ui->one->setStyleSheet("QLabel {}");
            ui->hz->setStyleSheet("QPushButton {background-color:yellow;}");
            ui->four->setStyleSheet("QLabel {background-color:yellow;}");
            ui->spulse->setStyleSheet("QPushButton {background-color:yellow;}");
        }else if(selectedType == "beta1"){
            selectedType = "alpha";
            selectedPulse = "spulse";
            ui->beta1->setStyleSheet("QPushButton {}");
            ui->two->setStyleSheet("QLabel {}");
            ui->alpha->setStyleSheet("QPushButton {background-color:yellow;}");
            ui->one->setStyleSheet("QLabel {background-color:yellow;}");
            ui->spulse->setStyleSheet("QPushButton {background-color:yellow;}");
        }else if(selectedType == "beta2"){
            selectedType = "beta1";
            selectedPulse = "spulse";
            ui->beta2->setStyleSheet("QPushButton {}");
            ui->three->setStyleSheet("QLabel {}");
            ui->beta1->setStyleSheet("QPushButton {background-color:yellow;}");
            ui->two->setStyleSheet("QLabel {background-color:yellow;}");
            ui->spulse->setStyleSheet("QPushButton {background-color:yellow;}");
        }else if(selectedType == "hz"){
            selectedType = "beta2";
            selectedPulse = "spulse";
            ui->hz->setStyleSheet("QPushButton {}");
            ui->four->setStyleSheet("QLabel {}");
            ui->beta2->setStyleSheet("QPushButton {background-color:yellow;}");
            ui->three->setStyleSheet("QLabel {background-color:yellow;}");
            ui->spulse->setStyleSheet("QPushButton {background-color:yellow;}");
        }
    } else {
        if(currentSession->getIntensity() == 8){
            currentSession->setIntensity(-1);
            ui->eight->setStyleSheet("QLabel {}");
            ui->seven->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 7){
            currentSession->setIntensity(-1);
            ui->seven->setStyleSheet("QLabel {}");
            ui->six->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 6){
            currentSession->setIntensity(-1);
            ui->six->setStyleSheet("QLabel {}");
            ui->five->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 5){
            currentSession->setIntensity(-1);
            ui->five->setStyleSheet("QLabel {}");
            ui->four->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 4){
            currentSession->setIntensity(-1);
            ui->four->setStyleSheet("QLabel {}");
            ui->three->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 3){
            currentSession->setIntensity(-1);
            ui->three->setStyleSheet("QLabel {}");
            ui->two->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 2){
            currentSession->setIntensity(-1);
            ui->two->setStyleSheet("QLabel {}");
            ui->one->setStyleSheet("QLabel {background-color:yellow;}");
        } else if (currentSession->getIntensity() == 1){
            currentSession->setIntensity(7);
            ui->one->setStyleSheet("QLabel {}");
            ui->eight->setStyleSheet("QLabel {background-color:yellow;}");
        }
    }
}

void MainWindow::userLPressed(){
    //Decrease user selected time
    depleteBattery();
    if(userDesTime - 5 >= 20){
        userDesTime -= 5;
        ui->userTime->setText(QString::number(ui->userTime->text().toInt() - 5));
    }
}

void MainWindow::userRPressed(){
    //Increase user selected time
    depleteBattery();
    if(userDesTime + 5 <= 100){
        userDesTime += 5;
        ui->userTime->setText(QString::number(ui->userTime->text().toInt() + 5));
    }
}


void MainWindow:: selectPressed(){
    if(sessionRunning == false){

        //Starting a session
        nonMenuState = true;

        ui->intUpButton->setEnabled(false);
        ui->intDownButton->setEnabled(false);
        ui->selectButton->setEnabled(false);

        ui->userTimeL->setEnabled(false);
        ui->userTimeR->setEnabled(false);

        int duration = 0;
        if(selectedSession == "20"){
            duration = 20;
        } else if(selectedSession == "45"){
            duration = 45;
        } else if(selectedSession == "User"){
            duration = userDesTime;
        }

        QString pulse = "";

        if(selectedType == "alpha"){
            pulse = "short";
            ui->one->setStyleSheet("QLabel {}");
        } else if (selectedType == "beta1"){

            pulse = "short";
            ui->two->setStyleSheet("QLabel {}");
        } else if (selectedType == "beta2"){
            pulse = "short";
            ui->three->setStyleSheet("QLabel {}");
        } else if (selectedType == "hz"){
            pulse = "short";
            ui->four->setStyleSheet("QLabel {}");
        }

        //Blink element before running test

        currentSession = new Session(duration, pulse, selectedType);

        QTimer* count = new QTimer();
        count->setSingleShot(true);
        count->setInterval(500);

        connect(count, SIGNAL(timeout()), this, SLOT (blinkElement()));

        count->start();

        QTimer* sessionTimer = new QTimer();
        sessionTimer->setSingleShot(true);
        sessionTimer->setInterval(5000);

        connect(sessionTimer, SIGNAL(timeout()), this, SLOT(connectionTest()));
        sessionTimer->start();
    } else {
        //Adding to preferences during session

        if(selectedType == "alpha"){
            db->addPreference("alpha", currentSession->getIntensity());
        } else if (selectedType == "beta1"){
            db->addPreference("beta1", currentSession->getIntensity());
        } else if (selectedType == "beta2"){
            qInfo("happened");
            db->addPreference("beta2", currentSession->getIntensity());
        } else if (selectedType == "hz"){
            db->addPreference("hz", currentSession->getIntensity());
        }

        getPreferences();

        clearBattery();
        swapGraph();
        graphAnimation();

    }

}

void MainWindow:: updateBattery(){
    //Update the battery from admin panel
    battery = ui->batteryBox->value();
}

void MainWindow:: updateConnection(){
    //Update the connection from admin battery panel

    if(ui->strengthBox->currentText() == "Strong"){
        connectionStrength = "strong";
    } else if (ui->strengthBox->currentText() == "Ok"){
        connectionStrength = "okay";
    } else if (ui->strengthBox->currentText() == "Bad"){
        connectionStrength = "no";
    }

}

void MainWindow:: updateDisrupt(){
    //Disrupt connection during session from admin panel
    if(sessionRunning == true && disruptCheck == false){
        disruptCheck = true;

        //Pause all timers from the session
        for(int i = 0; i < timers.size(); i++){
            timers.at(i)->stop();
            if(timers.at(i)->remainingTime() >= 0){
                timers.at(i)->setInterval(timers.at(i)->remainingTime());
            }
        }

        if(batteryShowing == false){
            swapGraph();
        }
        clearBattery();

        ui->selectButton->setEnabled(false);

        int interval = 500;

        for(int i = 0; i < 3; i++){
            QTimer* noTimer = new QTimer();
            noTimer->setSingleShot(true);
            noTimer->setInterval(interval);
            interval += 500;
            connect(noTimer, SIGNAL(timeout()), this, SLOT (noConnection()));
            noTimer->start();
        }

        QTimer* animationTimer = new QTimer();
        animationTimer->setSingleShot(true);
        animationTimer->setInterval(2000);
        connect(animationTimer, SIGNAL(timeout()), this, SLOT (graphAnimation()));
        animationTimer->start();
    }
}

void MainWindow::disruptRestore(){
    //Restore session after disrupt

    disruptCheck = false;
    swapGraph();
    clearBattery();
    ui->selectButton->setEnabled(true);

    for(int i = 0; i < timers.size(); i++){
        timers.at(i)->start();
    }
}


void MainWindow:: blinkElement(){

    //Turn an element on, then after an interval turn it off
    if(testRunning == false){
        if(selectedType == "alpha"){
            ui->one->setStyleSheet("QLabel {background-color:yellow;}");
        } else if(selectedType == "beta1"){
            ui->two->setStyleSheet("QLabel {background-color:yellow;}");
        } else if(selectedType == "beta2"){
            ui->three->setStyleSheet("QLabel {background-color:yellow;}");
        } else if(selectedType == "hz"){
            ui->four->setStyleSheet("QLabel {background-color:yellow;}");
        }
    } else{
        if(selectedPulse == "spulse"){
            ui->spulse->setStyleSheet("QPushButton {}");

        } else if(selectedPulse == "duty"){
            ui->duty->setStyleSheet("QPushButton {}");
        }
    }

    QTimer* count = new QTimer();
    count->setSingleShot(true);
    count->setInterval(500);

    connect(count, SIGNAL(timeout()), this, SLOT (unblinkElement()));

    count->start();
}

void MainWindow:: unblinkElement(){

    //Turn off a UI element that was being blinked
    if(testRunning == false){
        if(selectedType == "alpha"){
            ui->one->setStyleSheet("QLabel {}");
        } else if(selectedType == "beta1"){
            ui->two->setStyleSheet("QLabel {}");
        } else if(selectedType == "beta2"){
            ui->three->setStyleSheet("QLabel {}");
        } else if(selectedType == "hz"){
            ui->four->setStyleSheet("QLabel {}");
        }
    } else if(testRunning == true) {

        //During a connection test the session starts from here

        clearBattery();
        swapGraph();

        if(selectedPulse == "spulse"){
            ui->spulse->setStyleSheet("QPushButton {background-color:yellow;}");
        } else if(selectedPulse == "duty"){
            ui->duty->setStyleSheet("QPushButton {background-color:yellow;}");
        }

        if(connectionStrength == "strong"){
            ui->one_2->setStyleSheet("QLabel {background-color:green;}");
            ui->two_2->setStyleSheet("QLabel {background-color:green;}");
            ui->three_2->setStyleSheet("QLabel {background-color:green;}");

        } else if(connectionStrength == "okay"){
            ui->four_2->setStyleSheet("QLabel {background-color:yellow;}");
            ui->five_2->setStyleSheet("QLabel {background-color:yellow}");
            ui->six_2->setStyleSheet("QLabel {background-color:yellow}");

        } else if(connectionStrength == "no"){

            //Do a graph animation then start the session to simulate a bad connection
            ui->seven_2->setStyleSheet("QLabel {background-color:red}");
            ui->eight_2->setStyleSheet("QLabel {background-color:red}");

            int interval = 500;

            for(int i = 0; i < 3; i++){
                QTimer* noTimer = new QTimer();
                noTimer->setSingleShot(true);
                noTimer->setInterval(interval);
                interval += 500;
                connect(noTimer, SIGNAL(timeout()), this, SLOT (noConnection()));
                noTimer->start();
            }

            QTimer* animationTimer = new QTimer();
            animationTimer->setSingleShot(true);
            animationTimer->setInterval(2000);
            connect(animationTimer, SIGNAL(timeout()), this, SLOT (graphAnimation()));
            animationTimer->start();

            return;
        }

        QTimer* strengthTimer = new QTimer();
        strengthTimer->setSingleShot(true);
        strengthTimer->setInterval(1750);

        connect(strengthTimer, SIGNAL(timeout()), this, SLOT (swapGraph()));

        strengthTimer->start();

        QTimer* sessionBegin = new QTimer();
        sessionBegin->setSingleShot(true);
        sessionBegin->setInterval(2000);

        connect(sessionBegin, SIGNAL(timeout()), this, SLOT (beginSession()));

        sessionBegin->start();
    }
}

void MainWindow:: noConnection(){

    //Switch between red and off to show bad connection

    if(ui->seven_2->styleSheet() == "QLabel {background-color:red}"){
        ui->seven_2->setStyleSheet("QLabel {}");
        ui->eight_2->setStyleSheet("QLabel {}");
    } else {
        ui->seven_2->setStyleSheet("QLabel {background-color:red}");
        ui->eight_2->setStyleSheet("QLabel {background-color:red}");
    }
}

void MainWindow:: graphAnimation(){
    //Animate the graph by moving between the numbers

    ui->one_2->setStyleSheet("QLabel {background-color:yellow;}");
    ui->left->setStyleSheet("QPushButton {background-color:yellow;}");
    ui->right->setStyleSheet("QPushButton {background-color:yellow;}");

    clearBattery();

    //Call animationSwitch to move between the numbers
    int interval = 250;
    for(int i = 0; i < 8; i++){
        QTimer* graphTimer = new QTimer();
        graphTimer->setSingleShot(true);
        graphTimer->setInterval(interval);
        interval += 250;
        connect(graphTimer, SIGNAL(timeout()), this, SLOT (animationSwitch()));
        graphTimer->start();
    }

    if(sessionRunning == false){
        //Start the session

        QTimer* sessionBegin = new QTimer();
        sessionBegin->setSingleShot(true);
        sessionBegin->setInterval(2100);

        connect(sessionBegin, SIGNAL(timeout()), this, SLOT (beginSession()));

        QTimer* badConnectionTimer = new QTimer();
        badConnectionTimer->setSingleShot(true);
        badConnectionTimer->setInterval(2100);

        connect(badConnectionTimer, SIGNAL(timeout()), this, SLOT (swapGraph()));

        badConnectionTimer->start();
        sessionBegin->start();
    } else if (disruptCheck == false){
        //Stop the preference animation

        QTimer* preferenceEnd = new QTimer();
        preferenceEnd->setSingleShot(true);
        preferenceEnd->setInterval(2100);
        connect(preferenceEnd, SIGNAL(timeout()), this, SLOT (swapGraph()));
        preferenceEnd->start();
    } else {
        //Stop the disruption animation

        QTimer* disruptEnd = new QTimer();
        disruptEnd->setSingleShot(true);
        disruptEnd->setInterval(2100);
        connect(disruptEnd, SIGNAL(timeout()), this, SLOT (disruptRestore()));
        disruptEnd->start();
    }
}

void MainWindow:: animationSwitch(){

    //Switch between the numbers (on the battery graph)

    if(ui->one_2->styleSheet() == "QLabel {background-color:yellow;}"){
        ui->one_2->setStyleSheet("QLabel {}");
        ui->two_2->setStyleSheet("QLabel {background-color:yellow;}");
    } else if(ui->two_2->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->two_2->setStyleSheet("QLabel {}");
        ui->three_2->setStyleSheet("QLabel {background-color:yellow;}");

    } else if(ui->three_2->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->three_2->setStyleSheet("QLabel {}");
        ui->four_2->setStyleSheet("QLabel {background-color:yellow;}");

    } else if(ui->four_2->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->four_2->setStyleSheet("QLabel {}");
        ui->five_2->setStyleSheet("QLabel {background-color:yellow;}");

    } else if(ui->five_2->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->five_2->setStyleSheet("QLabel {}");
        ui->six_2->setStyleSheet("QLabel {background-color:yellow;}");

    } else if(ui->six_2->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->six_2->setStyleSheet("QLabel {}");
        ui->seven_2->setStyleSheet("QLabel {background-color:yellow;}");

    } else if(ui->seven_2->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->seven_2->setStyleSheet("QLabel {}");
        ui->eight_2->setStyleSheet("QLabel {background-color:yellow;}");

    } else if(ui->eight_2->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->eight_2->setStyleSheet("QLabel {}");
        ui->one_2->setStyleSheet("QLabel {background-color:yellow;}");
    }
}

void MainWindow:: softOffAnimation(){
    //Switch between the numbers (in decreasing order and on the intensity graph)

    if(ui->eight->styleSheet() == "QLabel {background-color:yellow;}"){
        ui->eight->setStyleSheet("QLabel {}");
        ui->seven->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(-1);
    } else if(ui->seven->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->seven->setStyleSheet("QLabel {}");
        ui->six->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(-1);
    } else if(ui->six->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->six->setStyleSheet("QLabel {}");
        ui->five->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(-1);
    } else if(ui->five->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->five->setStyleSheet("QLabel {}");
        ui->four->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(-1);
    } else if(ui->four->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->four->setStyleSheet("QLabel {}");
        ui->three->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(-1);
    } else if(ui->three->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->three->setStyleSheet("QLabel {}");
        ui->two->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(-1);
    } else if(ui->two->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->two->setStyleSheet("QLabel {}");
        ui->one->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(-1);
    }
}

void MainWindow:: softOnAnimation(){
    //Switch between the numbers increasingly (on the intensity graph)

    if(ui->one->styleSheet() == "QLabel {background-color:yellow;}"){
        ui->one->setStyleSheet("QLabel {}");
        ui->two->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(1);
    } else if(ui->two->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->two->setStyleSheet("QLabel {}");
        ui->three->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(1);
    } else if(ui->three->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->three->setStyleSheet("QLabel {}");
        ui->four->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(1);
    } else if(ui->four->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->four->setStyleSheet("QLabel {}");
        ui->five->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(1);
    } else if(ui->five->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->five->setStyleSheet("QLabel {}");
        ui->six->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(1);
    } else if(ui->six->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->six->setStyleSheet("QLabel {}");
        ui->seven->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(1);
    } else if(ui->seven->styleSheet() == "QLabel {background-color:yellow;}") {
        ui->seven->setStyleSheet("QLabel {}");
        ui->eight->setStyleSheet("QLabel {background-color:yellow;}");
        currentSession->setIntensity(1);
    }
}


void MainWindow:: connectionTest(){
    //Start the connecton test

    testRunning = true;

    QTimer* count = new QTimer();
    count->setSingleShot(true);
    count->setInterval(500);

    connect(count, SIGNAL(timeout()), this, SLOT (blinkElement()));

    count->start();
}

void MainWindow:: softOff(){
    //Slowly decrease intensity until device is off

    ui->intDownButton->setEnabled(false);
    ui->intUpButton->setEnabled(false);
    ui->selectButton->setEnabled(false);

    for(int i = 0; i < timers.size(); i++){
        timers.at(i)->stop();
    }
    timers.clear();


    if(batteryShowing == true){
        swapGraph();
    }

    int totalTime = currentSession->getIntensity() * 250;

    int interval = 250;

    //Animate the soft off

    for(int i = 0; i < currentSession->getIntensity(); i++){
        QTimer* graphTimer = new QTimer();
        graphTimer->setSingleShot(true);
        graphTimer->setInterval(interval);
        interval += 250;
        connect(graphTimer, SIGNAL(timeout()), this, SLOT (softOffAnimation()));
        graphTimer->start();

    }

    QTimer* endTimer = new QTimer();
    endTimer->setSingleShot(true);
    endTimer->setInterval(totalTime + 250);

    connect(endTimer, SIGNAL(timeout()), this, SLOT (endSession()));

    endTimer->start();
}

void MainWindow:: softOn(int inten){
    //Slowly increase intensity until preferered intensity reached

    //int totalTime = inten * 250;
    int interval = 250;

    for(int i = 0; i < inten-1; i++){
        QTimer* graphTimer = new QTimer();
        graphTimer->setSingleShot(true);
        graphTimer->setInterval(interval);
        interval += 250;
        connect(graphTimer, SIGNAL(timeout()), this, SLOT (softOnAnimation()));
        graphTimer->start();
    }
}


void MainWindow:: saveSession(){
    //Save the session in the database

    ui->saveButton->setEnabled(false);
    db->addSession(selectedType, currentSession->getDuration(), currentSession->getIntensity());

    getSessionHistory();
}

void MainWindow:: beginSession(){
    //Soft on with the preferred intensity if there is one

    if(selectedType == "alpha"){
        if(typePreferences.at(0)->getIntensity() > 1){
            int inten = typePreferences.at(0)->getIntensity();
            softOn(inten);
        }
    } else if (selectedType == "beta1"){
        if(typePreferences.at(1)->getIntensity() > 1){
            int inten = typePreferences.at(1)->getIntensity();
            softOn(inten);
        }
    } else if (selectedType == "beta2"){
        if(typePreferences.at(2)->getIntensity() > 1){
            int inten = typePreferences.at(2)->getIntensity();
            softOn(inten);
        }
    } else if (selectedType == "hz"){
        if(typePreferences.at(3)->getIntensity() > 1){
            int inten = typePreferences.at(3)->getIntensity();
            softOn(inten);
        }
    }

    //Set iniital state of ui elements

    ui->saveButton->setEnabled(true);

    nonMenuState = false;

    ui->intUpButton->setEnabled(true);
    ui->intDownButton->setEnabled(true);
    ui->selectButton->setEnabled(true);

    ui->one->setStyleSheet("QLabel {background-color:yellow;}");
    ui->left->setStyleSheet("QPushButton {}");
    ui->right->setStyleSheet("QPushButton {}");

    testRunning = false;
    sessionRunning = true;

    //Set the timer for the session to end dynamically based on the duration selected

    QTimer* sessionEnd = new QTimer();
    sessionEnd->setSingleShot(true);
    sessionEnd->setInterval(currentSession->getDuration() * 1000);
    connect(sessionEnd, SIGNAL(timeout()), this, SLOT (softOff()));
    sessionEnd->start();

    timers.append(sessionEnd);

    int sessDuration = currentSession->getDuration();
    int ctr = sessDuration/5;
    double interval = (sessDuration/(qFloor(sessDuration/5))*1000);
    double statInterval = interval;

    //Create timers that display the battery and decrease it at certain intervals
    for(int i = 0; i < ctr; i++){
        QTimer* batteryTimer = new QTimer();
        batteryTimer->setSingleShot(true);
        batteryTimer->setInterval(interval);

        QTimer* depleteTimer = new QTimer();
        depleteTimer->setSingleShot(true);
        depleteTimer->setInterval(interval);

        timers.append(batteryTimer);
        timers.append(depleteTimer);

        interval += statInterval;

        connect(batteryTimer, SIGNAL(timeout()), this, SLOT (displayBattery()));
        connect(batteryTimer, SIGNAL(timeout()), this, SLOT (depleteBattery()));

        batteryTimer->start();
        depleteTimer->start();
    }
}

void MainWindow:: endSession(){
    //Ending a session, restore all ui elements to their menu state

    sessionRunning = false;

    clearIntensity();

    ui->intDownButton->setEnabled(true);
    ui->intUpButton->setEnabled(true);
    ui->selectButton->setEnabled(true);

    ui->saveButton->setEnabled(false);

    ui->alpha->setStyleSheet("QPushButton {background-color:yellow;}");
    ui->beta1->setStyleSheet("QPushButton {}");
    ui->beta2->setStyleSheet("QPushButton {}");
    ui->hz->setStyleSheet("QPushButton {}");

    ui->twenty->setStyleSheet("QPushButton {background-color:yellow;}");
    ui->forty->setStyleSheet("QPushButton {}");
    ui->user->setStyleSheet("QPushButton {}");

    ui->one->setStyleSheet("QLabel {background-color:yellow;}");

    selectedType = "alpha";
    selectedPulse = "spulse";
    selectedSession = "20";

    if(batteryShowing == true){
        swapGraph();
    }
}


