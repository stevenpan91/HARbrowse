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
#include <QStyle>

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent)
{
    //Main window
    this->resize(WIN_X_SIZE,WIN_Y_SIZE);
    this->setWindowTitle("HARbrowse");
    this->setToolTip("This is a test");
    //this->mainWinState = Min;
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    QStyle *style = qApp->style();

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
    QIcon closeIcon=style->standardIcon(QStyle::SP_TitleBarCloseButton);
    quit = new QPushButton(this);
    //quit->setText("X");
    quit->setIcon(closeIcon);
    quit->resize(20,20);
    quit->move(980,0);
    quit->setAutoDefault(false);
    quit->setDefault(false);
    quit->setFont(QFont("Times",8));
    quit->setStyleSheet(".QPushButton{border: 0.5px solid black; border-radius: 5px;}");
    QObject::connect(quit, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));

    //Minimize section
    QIcon minIcon = style->standardIcon(QStyle::SP_TitleBarMinButton);
    minimize = new QPushButton(this);
    //minimize->setText("_");
    minimize->setIcon(minIcon);
    minimize->resize(20,20);
    minimize->move(940,0);
    minimize->setAutoDefault(false);
    minimize->setDefault(false);
    minimize->setFont(QFont("Times",8));

    minimize->setStyleSheet(".QPushButton{border: 0.5px solid black; border-radius: 5px;}");
    connect(minimize,SIGNAL(clicked()),this,SLOT(WinMinimize()));

    //Minimize and Maximize window
    QIcon maxIcon = style->standardIcon(QStyle::SP_TitleBarMaxButton);
    minmaxtoggle = new QPushButton(this);
    minmaxtoggle->setIcon(maxIcon);
    minmaxtoggle->resize(20,20);
    minmaxtoggle->move(960,0);
    minmaxtoggle->setAutoDefault(false);
    minmaxtoggle->setDefault(false);
    minmaxtoggle->setFont(QFont("Times",8));
    minmaxtoggle->setStyleSheet(".QPushButton{border: 0.5px solid black; border-radius: 5px;}");
    connect(minmaxtoggle,SIGNAL(clicked()),this,SLOT(WinMinMaxToggle()));


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
   this->setWindowState(Qt::WindowMinimized);

}

void MainWindow::WinMinMaxToggle(){
    //QTextStream out(stdout);

    this->setWindowState(this->windowState() ^ Qt::WindowMaximized);
    if (this->windowState()==Qt::WindowMaximized){
    //out<< "I am indeed Maximized";
    this->mainWinState=Max;}
    else{
    //out<< "I am indeed Minimized";
    this->mainWinState=Min;
    }

}
/*
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
*/

//taken from stack overflow 
bool urlExists(QUrl theurl){
    QTextStream out(stdout);
    QTcpSocket socket;
    QByteArray buffer;

        socket.connectToHost(theurl.host(), 80);
        if (socket.waitForConnected()) {
            //Standard http request
            socket.write("GET /index.html HTTP/1.1\r\n"
                     "host: " + theurl.host().toUtf8() + "\r\n\r\n");
            if (socket.waitForReadyRead()) {
                while(socket.bytesAvailable()){
                    //QByteArray bytes = socket.readAll();
                    buffer.append(socket.readAll());
                    //int packetSize = getPacketSize(buffer);
                    int packetSize=buffer.size();
                    while(packetSize>0)
                    {
                        //Output server response for debugging
                        out << "[" << buffer.data() << "]" <<endl;

                        //set Url if 200, 301, or 302 response given assuming that server will redirect
                        if (buffer.contains("200 OK") ||
                            buffer.contains("302 Found") ||
                            buffer.contains("301 Moved")) {
                            return true;
                        }  
                        buffer.remove(0,packetSize);
                        //packetSize=getPacketSize(buffer);
                        packetSize=buffer.size();

                    } //while packet size >0
                } //while socket.bytesavail
                        
            } //socket wait for ready read
        }//socket write   
    
    return false;
}


void MainWindow::launchURL()
{
   /*if (lineEdit1->text().toStdString().substr(0,7)!="http://"){
       std::string editstr="http://"+lineEdit1->text().toStdString();
       QString qeditstr = QString::fromStdString(editstr);
       lineEdit1->setText(qeditstr); 
   }*/
   
   
   QUrl url = QUrl::fromUserInput(lineEdit1->text()); 
   if (urlExists(url)){
       view->setUrl(url);}
   else{
       qDebug() << QString("Invalid URL: %1").arg(url.toString());
       displayErrorHTML();    
       }

}

void MainWindow::displayErrorHTML(){
       std::string path="home/steven/Projects/HARbrowse";
       std::string fullpath="file:///"+path+"/urlinvalid.html";
       view->load(QUrl::fromUserInput(QString::fromStdString(fullpath)));    
           //display error html page


}


void MainWindow::hideEvent(QHideEvent *event){
}

void MainWindow::showEvent(QShowEvent *event){
    //QTextStream out(stdout);
     
    if(this->mainWinState==Max){
    //    out<<"I come out as Max";
    
        this->showMaximized();}
    else if(this->mainWinState==Min){
    //    out<<"I come out as Min";
        this->setWindowState(Qt::WindowNoState);
    //    this->showNormal();
        }
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
   minmaxtoggle->move(nwidth-40,0);
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
