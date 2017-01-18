#include "mainwindow.h"
//#include <boost/filesystem.hpp>
#include <QFrame>
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
#include <QVBoxLayout>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent)
{
    //Main window
    this->resize(WIN_X_SIZE,WIN_Y_SIZE);
    this->setWindowTitle("HARbrowse");
    this->setToolTip("This is a test");
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    QStatusBar *statusBarWidget = new QStatusBar(this);
    statusBarWidget->move(0,0);
    statusBarWidget->resize(20,20);
    statusBarWidget->setSizeGripEnabled(true);

    //Webview
    view = new QWebView(this);
    view->resize(1000,520);
    view->move(0,50);
    view->setUrl(QUrl("http://google.com"));
    

    //Quit button section
    quit = new QPushButton(this);
    quit->setText("X");

    quit->resize(20,20);
    quit->move(980,0);
    quit->setAutoDefault(false);
    quit->setDefault(false);
    quit->setFont(QFont("Times",8));
    quit->setStyleSheet(".QPushButton{border: 0.5px solid black; border-radius: 5px;}");
    QObject::connect(quit, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));

    //Minimize section
    minimize = new QPushButton(this);
    minimize->setText("_");
    minimize->resize(20,20);
    minimize->move(940,0);
    minimize->setAutoDefault(false);
    minimize->setDefault(false);
    minimize->setFont(QFont("Times",8));

    minimize->setStyleSheet(".QPushButton{border: 0.5px solid black; border-radius: 5px;}");
    connect(minimize,SIGNAL(clicked()),this,SLOT(WinMinimize()));

    //URL section
    lineEdit1 = new QLineEdit(this);
    lineEdit1->resize(800,20);
    lineEdit1->move(20,20);
    lineEdit1->setStyleSheet(".QLineEdit{border-radius: 10px;}");

    //Launch URL section
    urlLaunch = new QPushButton(this);
    urlLaunch->setText("Launch");
    urlLaunch->resize(70,20);
    urlLaunch->move(820,20);
    urlLaunch->setShortcut(QKeySequence(Qt::Key_Enter));
    urlLaunch->setDefault(true);
    urlLaunch->setAutoDefault(true);
    urlLaunch->setFont(QFont("Times",10));
    urlLaunch->setStyleSheet(".QPushButton{border: 0.5px solid black; border-radius: 5px;}");
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

void MainWindow::WinMinimize()
{
   QTextStream out(stdout);
   out << "I'm Here";
   this->setWindowState(this->windowState() & Qt::WindowMinimized);
}

bool urlExists(QUrl theurl){
    QTextStream out(stdout);
    QTcpSocket socket;
    socket.connectToHost(theurl.host(), 80);
    if (socket.waitForConnected()) {
        socket.write("HEAD" + theurl.path().toUtf8() + " HTTP/1.1\r\n"
                     "Host: " + theurl.host().toUtf8() + "\r\n\r\n");
        if (socket.waitForReadyRead()) {
            QByteArray bytes = socket.readAll();
            if (bytes.contains("200 OK")) {
                out << "whassup\n";
                return true;
            }
        }
    }
    return false;
}

void MainWindow::launchURL()
{
   if (lineEdit1->text().toStdString().substr(0,7)!="http://"){
       std::string editstr="http://"+lineEdit1->text().toStdString();
       QString qeditstr = QString::fromStdString(editstr);
       lineEdit1->setText(qeditstr); 
   }
   
       QUrl url = QUrl::fromUserInput(lineEdit1->text()); 
      
       if (urlExists(url)){
       view->setUrl(url);}
       else{
           
       std::string path="home/steven/Projects/HARbrowse";
       std::string fullpath="file:///"+path+"/urlinvalid.html";
       view->load(QUrl::fromUserInput(QString::fromStdString(fullpath)));    
           //display error html page
       }

       view->setUrl(url);

       qDebug() << QString("Invalid URL: %1").arg(url.toString());

}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if( event->key() == Qt::Key_Return)
    {
        launchURL();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event){
   QTextStream out(stdout);
   //out << "I'm here."; 
   QSize MWSize = this->size();
   int nheight = MWSize.height();
   int nwidth = MWSize.width();
   view->resize(nwidth,nheight-80);
   lineEdit1->resize(nwidth-200,20);
   urlLaunch->move(nwidth-180,20);
   quit->move(nwidth-20,0);
   minimize->move(nwidth-60,0);
   /*out << nheight;
   out << " , "; 
   out << nwidth;
   out << "\n";*/
                
}


//Below two classes from Qt Shaped Clock example
void MainWindow::mousePressEvent(QMouseEvent *event){
//From Qt Documentation:
//Reason why pos() wasn't working is because the global
//position at time of event may be very different
//This is why the mpos = event->pos(); line before was
//possibly causing jumping behavior

    if (event->button() == Qt::LeftButton){
        mpos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    
    }

}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if (event->buttons()==Qt::LeftButton){
        move(event->globalPos() - mpos);
        event->accept();
    }

}
