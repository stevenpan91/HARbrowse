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
    this->setMouseTracking(true); //for resize event
    QStyle *style = qApp->style();

    /*
    QStatusBar *statusBarWidget = new QStatusBar(this);
    statusBarWidget->move(0,0);
    statusBarWidget->resize(10,10);
    //statusBarWidget->hide();
    statusBarWidget->setSizeGripEnabled(true);
    */

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
    quit->move(975,5);
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
    minimize->move(935,5);
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
    minmaxtoggle->move(955,5);
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
  std::string urlstring = lineEdit1->text().toStdString();
   /*if (urlstring.substr(0,11)!="http://www."){
       //initialize manipulated string
       std::string editstr;
       //if there is an http but no www. then
       if(urlstring.substr(0,7)=="http://"){
            editstr="http://www."+urlstring.substr(7,urlstring.length()-7);
       
       }
       //If there is www. but no http
       else if(urlstring.substr(0,4)=="www."){
            editstr="http://"+urlstring;
       
       }
       else{ 
            editstr="http://www."+lineEdit1->text().toStdString();
       }
       QString qeditstr = QString::fromStdString(editstr);
       lineEdit1->setText(qeditstr); 
   }*/

   if (urlstring.substr(0,7)!="http://"){
       std::string editstr;

       editstr="http://"+urlstring;
       QString qeditstr = QString::fromStdString(editstr);
       lineEdit1->setText(qeditstr); 
   
   }
   
   
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
                //int adjXdiff = -1*(rs_global_mpos.x() - global_mpos.x());
                
                //int adjYdiff = (event->globalPos().y() - global_mpos.y());
                
                //QPoint adjGlobalDifference = QPoint(adjXdiff,adjYdiff);
                //QPoint movePoint(mpos.x() - adjXdiff, mpos.y());
                //move(rs_global_mpos - movePoint);
   QSize MWSize = this->size();
   int nheight = MWSize.height();
   int nwidth = MWSize.width();
   view->resize(nwidth,nheight-80);
   lineEdit1->resize(nwidth-200,20);
   urlLaunch->move(nwidth-180,20);
   quit->move(nwidth-25,5);
   minmaxtoggle->move(nwidth-45,5);
   minimize->move(nwidth-65,5);
   /*out << nheight;
   out << " , "; 
   out << nwidth;
   out << "\n";*/
                
}


//Below two classes partially from Qt Shaped Clock example
void MainWindow::mousePressEvent(QMouseEvent *event){
//From Qt Documentation:
//Reason why pos() wasn't working is because the global
//position at time of event may be very different
//This is why the mpos = event->pos(); line before was
//possibly causing jumping behavior

    if (event->button() == Qt::LeftButton){
        //Coordinates have been mapped such that the mouse position is relative to the
        //upper left of the main window
        mpos = event->globalPos() - frameGeometry().topLeft();
        global_mpos = event->globalPos();
        storeWidth = this->width();
        storeHeight= this->height();
        event->accept();
    
    }

}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
   
    ResizeDirection rs_dir;

    //mapped mouse relative to upper left of window
    rs_mpos=event->globalPos()-frameGeometry().topLeft();//general position tracker for resizing
    rs_global_mpos=event->globalPos();
    QTextStream out(stdout);
    
    bool resizeZone=false;
    //upper left corner resize implementation
    if ( (abs(rs_mpos.x()) < 10 && abs(rs_mpos.y()) < 10) ||
         (abs(rs_mpos.x()) > this->width()-10 && abs(rs_mpos.y()) <10)  
         ){
        
        resizeZone=true;
        
        //Below for debugging
        /*
        out << rs_mpos.x() << " , " << rs_mpos.y() << "\n";
        out << "window: " << this->width() << " , " << this->height() << "\n";
        out << "globalpos: " << event->globalPos().x() << " , " 
            << event->globalPos().y() << "\n";
        */
    }

    if ( (abs(rs_mpos.x()) < 10 && abs(rs_mpos.y()) < 10)){
            this->setCursor(Qt::SizeFDiagCursor);
    
            if (event->buttons()==Qt::LeftButton && resizeZone==true){

                QPoint globalDifference = event->globalPos()-global_mpos;
                move(event->globalPos() - mpos);
                resize(storeWidth - globalDifference.x()
                    ,storeHeight -globalDifference.y());
                event->accept();
                resizeZone=false;
            }

    }
    else if(abs(rs_mpos.x()) > this->width()-10 && abs(rs_mpos.y()) <10){
        this->setCursor(Qt::SizeBDiagCursor);
    
    

            if (event->buttons()==Qt::LeftButton && resizeZone==true){
                
                //flip over x sign because now relative to upper right
                int adjXdiff = -1*(event->globalPos().x() - global_mpos.x());
                
                int adjYdiff = (event->globalPos().y() - global_mpos.y());
                
                QPoint adjGlobalDifference = QPoint(adjXdiff,adjYdiff);
                QPoint movePoint(mpos.x() - adjXdiff, mpos.y());
                //move(event->globalPos()-movePoint);
                move(event->globalPos()-mpos);
                resize(storeWidth-adjXdiff, storeHeight-adjYdiff);
                event->accept();
                resizeZone=false;
            }

    }

    //in any move event if it is not in a resize region use the default cursor
    else{
    
        this->setCursor(Qt::ArrowCursor);
    }
    
    //manual resize section
    /*if (event->buttons()==Qt::LeftButton && resizeZone==true){
        
        //move then resize to difference of old position
        //jumping behavior
        
        QPoint globalDifference = event->globalPos()-global_mpos;
        move(event->globalPos() - mpos);
        resize(storeWidth - globalDifference.x()
                ,storeHeight -globalDifference.y());
        event->accept();
        resizeZone=false;
    }*/

    //simple move section
    if (event->buttons()==Qt::LeftButton && resizeZone==false){
        move(event->globalPos() - mpos);
        event->accept();
    }
    

}
