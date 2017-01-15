#include "mainwindow.h"
#include <QCoreApplication>
#include <QApplication>
#include <QFont>
#include <QPushButton>
#include <QTextStream>
#include <QFile>
#include <QLineEdit>
#include <QtWebKit>
#include <QString>
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

void MainWindow::launchURL()
{
   if (lineEdit1->text().toStdString().substr(0,7)!="http://"){
       const std::string editstr="http://"+lineEdit1->text().toStdString(); 
       QString qeditstr = QString::fromStdString(editstr);
       
       //view->setUrl(QUrl(lineEdit1->setText(qeditstr)));
   
   } 
   
   
   view->setUrl(QUrl(lineEdit1->text())); 


}

