#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QtWidgets>
#include <QString>
#include <QTextCodec>
#include <QMediaPlayer>
#include <QTimer>

#include <stdio.h>  /* defines FILENAME_MAX */
#include <unistd.h>
#include <iostream>
#include <time.h>
#define GetCurrentDir getcwd

using namespace std;

string GetCurrentWorkingDir(void)
{
  char buff[FILENAME_MAX];
  GetCurrentDir( buff, FILENAME_MAX );
  string current_working_dir(buff);
  return current_working_dir;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serialBuffer = "";

    string movie_dir = "/pictures/titlescreen.gif"; //directory of gif folder. forward slash due to movie formatting
    string current_dir = GetCurrentWorkingDir();

    // gets the string of current directory, replaces every backslash with forward slash due to pixmap and movie formatting
    // searches for "\\" instead of "\" due to escape sequence conflict like "\n"
    size_t found = current_dir.find_first_of("\\");
    while (found!=std::string::npos)
      {
        current_dir[found]='/';
        found=current_dir.find_first_of("\\",found+1);
      }

    string display_movie = current_dir + movie_dir;

    QString qstr_movie = QString::fromStdString(display_movie);     // converts a string to QString, Qmovie requires QString


    QMovie *movie = new QMovie(qstr_movie);

    ui->label_pic->setMovie(movie);
    movie->start();


    /*
     * #######################      CODE FOR DISPLAYING PICTURES    #############################


    string floorplan_dir = "/pictures/tables/table13.png"; //directory of pictures folder. forward slash due to pixmap formatting
    //string tables_dir = "/pictures/tables";
    string current_dir = GetCurrentWorkingDir();


    //appends the current directory with the directory of picture folder
    string display_pic = current_dir + floorplan_dir;
    QString qstr = QString::fromStdString(display_pic);     // converts a string to QString, pixmap requires QString


    //QPixmap pix("C:/Users/danie/Documents/GitHub/cse453-RFID-Wedding/build-cse453wedding-Desktop_Qt_5_10_1_MinGW_32bit-Debug/pictures/basefloor.jpg");
    QPixmap pix(qstr);
    ui->label_pic->setPixmap(pix);
    */

    arduino_is_available = false;
    arduino_port_name = "";
    arduino = new QSerialPort;

    /*
     // Function to identify the Arduino vendor ID & product ID
     // Vendor & Product ID is used to identify and select the right port the Arduino is connected on computer
     // Run this function once on a new Arduino to get the IDs
     // Update the IDs in header file -> mainwindow.h

    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
        if(serialPortInfo.hasProductIdentifier()){
            qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
        }
        qDebug() << "Has product ID: " << serialPortInfo.hasProductIdentifier();
        if(serialPortInfo.hasProductIdentifier()){
            qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
        }
    }*/

    // Checks if Arduino is available
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            // Checks for arduino uno product id
            if(serialPortInfo.productIdentifier() == arduino_uno_product_id){
                arduino_port_name = serialPortInfo.portName();
                arduino_is_available = true;
            }
            // Checks for arduino mega product id
            else if(serialPortInfo.productIdentifier() == arduino_mega_product_id){
                arduino_port_name = serialPortInfo.portName();
                arduino_is_available = true;
            }
        }
    }

    // If arduino is available, open a serialport with the following configuration
    if(arduino_is_available){
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud115200);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        QObject::connect(arduino, SIGNAL(readyRead()),this, SLOT(readSerial()));
    }
    else{
        //give error message if not available
        QMessageBox::warning(this, "Port error", "Couldn't find the Arduino!");
    }
}

void MainWindow::readSerial()
{

    serialData = arduino->readAll();

    //qDebug() << serialData;

    valid_data = QString::fromStdString(serialData.toStdString());

    updateGUI(valid_data);

}


MainWindow::~MainWindow()
{
    if(arduino->isOpen()){
        arduino->close();
    }
    delete ui;
}


void MainWindow::updateGUI(QString valid_data)
{
    string current_dir = GetCurrentWorkingDir();

    // gets the string of current directory, replaces every backslash with forward slash due to pixmap and movie formatting
    // searches for "\\" instead of "\" due to escape sequence conflict like "\n"
    size_t found = current_dir.find_first_of("\\");
    string pic_dir;

    while (found!=std::string::npos)
    {
        current_dir[found]='/';
        found=current_dir.find_first_of("\\",found+1);
    }


    if(valid_data.contains("61") || valid_data.contains("a")) {             // table 1
        pic_dir = "/pictures/tables/table1.png";
    }
    else if(valid_data.contains("62") || valid_data.contains("b")) {        // table 2
        pic_dir = "/pictures/tables/table2.png";
    }
    else if(valid_data.contains("63") || valid_data.contains("c")) {        // table 3
        pic_dir = "/pictures/tables/table3.png";
    }
    else if(valid_data.contains("64") || valid_data.contains("d")) {        // table 4
        pic_dir = "/pictures/tables/table4.png";
    }
    else if(valid_data.contains("65") || valid_data.contains("e")) {        // table 5
        pic_dir = "/pictures/tables/table5.png";
    }
    else if(valid_data.contains("66") || valid_data.contains("f")) {        // table 6
        pic_dir = "/pictures/tables/table6.png";
    }
    else if(valid_data.contains("67") || valid_data.contains("g")) {        // table 7
        pic_dir = "/pictures/tables/table7.png";
    }
    else if(valid_data.contains("68") || valid_data.contains("h")) {        // table 8
        pic_dir = "/pictures/tables/table8.png";
    }
    else if(valid_data.contains("69") || valid_data.contains("i")) {        // table 9
        pic_dir = "/pictures/tables/table9.png";
    }
    else if(valid_data.contains("6A") || valid_data.contains("j")) {        // table 10
        pic_dir = "/pictures/tables/table10.png";
    }
    else if(valid_data.contains("6B") || valid_data.contains("k")) {        // table 11
        pic_dir = "/pictures/tables/table11.png";
    }
    else if(valid_data.contains("6C") || valid_data.contains("l")) {        // table 12
        pic_dir = "/pictures/tables/table12.png";
    }
    else if(valid_data.contains("6D") || valid_data.contains("m")) {        // table 13
        pic_dir = "/pictures/tables/table13.png";
    }
    else if(valid_data.contains("6E") || valid_data.contains("n")) {        // table 14
        pic_dir = "/pictures/tables/table14.png";
    }
    else if(valid_data.contains("6F") || valid_data.contains("o")) {        // table 15
        pic_dir = "/pictures/tables/table15.png";
    }

    else if(valid_data.contains("70") || valid_data.contains("p")) {        // table 1 winner
        pic_dir = "/pictures/tables/table1winner.png";
    }
    else if(valid_data.contains("71") || valid_data.contains("q")) {        // table 2 winner
        pic_dir = "/pictures/tables/table2winner.png";
    }
    else if(valid_data.contains("72") || valid_data.contains("r")) {        // table 3 winner
        pic_dir = "/pictures/tables/table3winner.png";
    }
    else if(valid_data.contains("73") || valid_data.contains("s")) {        // table 4 winner
        pic_dir = "/pictures/tables/table4winner.png";
    }
    else if(valid_data.contains("74") || valid_data.contains("t")) {        // table 5 winner
        pic_dir = "/pictures/tables/table5winner.png";
    }
    else if(valid_data.contains("75") || valid_data.contains("u")) {        // table 6 winner
        pic_dir = "/pictures/tables/table6winner.png";
    }
    else if(valid_data.contains("76") || valid_data.contains("v")) {        // table 7 winner
        pic_dir = "/pictures/tables/table7winner.png";
    }
    else if(valid_data.contains("77") || valid_data.contains("w")) {        // table 8 winner
        pic_dir = "/pictures/tables/table8winner.png";
    }
    else if(valid_data.contains("78") || valid_data.contains("x")) {        // table 9 winner
        pic_dir = "/pictures/tables/table9winner.png";
    }
    else if(valid_data.contains("79") || valid_data.contains("y")) {        // table 10 winner
        pic_dir = "/pictures/tables/table10winner.png";
    }
    else if(valid_data.contains("7A") || valid_data.contains("z")) {        // table 11 winner
        pic_dir = "/pictures/tables/table11winner.png";
    }
    else if(valid_data.contains("7B") || valid_data.contains("{")) {        // table 12 winner
        pic_dir = "/pictures/tables/table12winner.png";
    }
    else if(valid_data.contains("7C") || valid_data.contains("|")) {        // table 13 winner
        pic_dir = "/pictures/tables/table13winner.png";
    }
    else if(valid_data.contains("7D") || valid_data.contains("}")) {        // table 14 winner
        pic_dir = "/pictures/tables/table14winner.png";
    }
    else if(valid_data.contains("7E") || valid_data.contains("~")) {        // table 15 winner
        pic_dir = "/pictures/tables/table15winner.png";
    }


    string beep_dir = "/pictures/beep.mp3";
    string beep_beep = current_dir + beep_dir;
    QString qstr_beep = QString::fromStdString(beep_beep);     // converts a string to QString,

    QMediaPlayer *player = new QMediaPlayer;
    player->setMedia(QUrl(qstr_beep));
    player->play();

    //appends the current directory with the directory of picture folder
    string display_pic = current_dir + pic_dir;
    QString qstr = QString::fromStdString(display_pic);     // converts a string to QString, pixmap requires QString

    QPixmap pix(qstr);
    ui->label_pic->setPixmap(pix);

    //string display_movie2 = current_dir + movie_dir2;
    //QString qstr_movie2 = QString::fromStdString(display_movie2);     // converts a string to QString, Qmovie requires QString
    //QMovie *movie = new QMovie(qstr_movie2);

    //ui->label_pic->setMovie(movie);
    //movie->start();

    //QTimer::singleShot(5000, this, SLOT(idle_function()));


}

void MainWindow::idle_function()    // this function is meant to display the titlescreen after a certain amount of idle time
{
    string movie_dir = "/pictures/titlescreen.gif"; //directory of gif folder. forward slash due to movie formatting
    string current_dir = GetCurrentWorkingDir();

    // gets the string of current directory, replaces every backslash with forward slash due to pixmap and movie formatting
    // searches for "\\" instead of "\" due to escape sequence conflict like "\n"
    size_t found = current_dir.find_first_of("\\");
    while (found!=std::string::npos)
      {
        current_dir[found]='/';
        found=current_dir.find_first_of("\\",found+1);
      }

    string display_movie = current_dir + movie_dir;

    QString qstr_movie = QString::fromStdString(display_movie);     // converts a string to QString, Qmovie requires QString


    QMovie *movie = new QMovie(qstr_movie);

    ui->label_pic->setMovie(movie);
    movie->start();


}
