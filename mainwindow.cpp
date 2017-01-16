#include "mainwindow.h"
//#include <boost/filesystem.hpp>
#include <QCoreApplication>
#include <QApplication>
#include <QFont>
#include <QPushButton>
#include <QTextStream>
#include <QFile>
#include <QLineEdit>
#include <QtWebKit>
#include <QString>
#include <QDebug>
#include <QTcpSocket>
#include <string>

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent)
{
    //Main window
    this->resize(WIN_X_SIZE,WIN_Y_SIZE);
    this->setWindowTitle("HARbrowse");
    this->setToolTip("This is a test");
    
    //Webview
    view = new QWebView(this);
    view->resize(1000,520);
    view->move(0,50);
    view->setUrl(QUrl("http://google.com"));
    
    //Quit button section
    QPushButton *quit = new QPushButton(this);
    quit->setText("Quit");

    quit->resize(70,20);
    quit->move(930,580);
    quit->setFont(QFont("Times",12,QFont::Bold));
    QObject::connect(quit, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));

    //URL section
    lineEdit1 = new QLineEdit(this);
    lineEdit1->resize(800,20);
    lineEdit1->move(20,20);

    //Launch URL section
    QPushButton *urlLaunch = new QPushButton(this);
    urlLaunch->setText("Launch");
    urlLaunch->resize(70,20);
    urlLaunch->move(820,20);
    urlLaunch->setFont(QFont("Times",12,QFont::Bold));
    connect(urlLaunch,SIGNAL(clicked()),this,SLOT(launchURL()));

    //Print test
    /*qtextstream out(stdout);
    out << "whassup\n";*/
    
    lineEdit1->show();
    urlLaunch->show();
    quit->show();
    view->show();


    /*QFile data("myfile");

    if (data.open(QFile::WriteOnly)){
        QTextStream out(&data);
        out << "It's a beautiful morning." << endl;

    }
    */
    

}

bool urlExists(QUrl theurl){
    QTcpSocket socket;
    socket.connectToHost(theurl.host(), 80);
    if (socket.waitForConnected()) {
        socket.write("HEAD" + theurl.path().toUtf8() + " HTTP/1.1\r\n"
                     "Host: " + theurl.host().toUtf8() + "\r\n\r\n");
        if (socket.waitForReadyRead()) {
            QByteArray bytes = socket.readAll();
            if (bytes.contains("200 OK")) {
                return true;
            }
        }
    }
    return false;
}

void MainWindow::launchURL()
{
   if (lineEdit1->text().toStdString().substr(0,7)!="http://"){
       const std::string editstr="http://"+lineEdit1->text().toStdString(); 
       QString qeditstr = QString::fromStdString(editstr);
       lineEdit1->setText(qeditstr);
       QUrl url = QUrl::fromUserInput(lineEdit1->text()); 
       
       if (urlExists(url)){
       view->setUrl(url);}
       else{
           
       //view->load(QUrl("file:///home/steven/Projects/HARbrowse/urlinvalid.html"));    
       std::string path="home/steven/Projects/HARbrowse";
       std::string fullpath="file:///"+path+"/urlinvalid.html";
       view->load(QUrl::fromUserInput(QString::fromStdString(fullpath)));    
           //display error html page
       }
       
       qDebug() << QString("Invalid URL: %1").arg(url.toString());
       
   
   } 
   
   
   //view->setUrl(QUrl(lineEdit1->text())); 


}

