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
#include <QFileInfo>
#include <QTabWidget>
#include <QTextCodec>
#include <string>
#include <iostream>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QBitmap>
#include <QNetworkReply>
#include <QNetworkProxyFactory>
#include <QNetworkCookieJar>
#include <QStatusBar>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent)
{
    tabAmount=0;
    maxTabs=10;
    
    //Freed in destructor    
    views=(QWebView**)malloc(sizeof(QWebView*)*maxTabs);

    //Main window
    this->resize(WIN_X_SIZE,WIN_Y_SIZE);
    this->setWindowTitle("HARbrowse");
    this->setToolTip("This is a test");
    //this->mainWinState = Min;
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground,true);
    this->setMouseTracking(true); //for resize event
    //this->setStyleSheet("QMainWindow{background-color:rgba(150,255,255,100);border: 0.5px solid black; border-radius: 5px;}");
    QStyle *style = qApp->style();

    //Frame over main window to give round edge and background color
    
    frame=new QFrame(this);
    this->setCentralWidget(frame);
    frame->setMouseTracking(true);
    frame->resize(WIN_X_SIZE,WIN_Y_SIZE);
    frame->move(0,0);
    frame->setStyleSheet(".QFrame{background-color:rgba(0,120,100,180);border: 0px solid black; border-radius: 5px;}");
    

    //Update time display
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(showTime()));
    timer->start(1000); //1 second updates
    timeLabel = new QLabel(this);
    timeLabel->setMouseTracking(true);
    timeLabel->move(930,570);
    timeLabel->show(); 


    m_manager=new QNetworkAccessManager(this);
    
    //allow cookies for youtube
    //cookieJar=new QNetworkCookieJar();
    //m_manager->setCookieJar(cookieJar);

    connect(m_manager,SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    


    //Tab Page section
    tabControl = new QTabWidget(this);
    tabControl->setMouseTracking(true);
    tabControl->resize(1000,520);
    tabControl->move(0,50);
    tabControl->setAttribute(Qt::WA_TranslucentBackground,true);
    //tabControl->setStyleSheet("background:transparent");
    
    connect(tabControl,SIGNAL(currentChanged(int)),this, SLOT(updateUrlBar()));


    //Webview
    //view = new QWebView(this);
    view=new QWebView(tabControl);

    //backwards compatability to not have to redo all the code that says "view"
    views[0]=view;
    
    tabControl->addTab(view,QString::fromStdString(" "));
    tabAmount++;
    //view->resize(1000,520);
    //view->move(0,50);
    view->setMouseTracking(true);
    view->setStyleSheet("background:transparent");
    view->setAttribute(Qt::WA_TranslucentBackground,true);
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled,true);    
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages,true);    
    view->load(QUrl("http://www.google.com"));
    //view->page()->settings()->clearMemoryCaches();
    //view->reload();

    QToolButton *tb = new QToolButton(this);
    tb->setText("+");
    tabControl->addTab(new QLabel("Add tabs by pressing +"),QString());
    tabAmount++;
    tabControl->setTabEnabled(1,false);
    tabControl->tabBar()->setTabButton(1,QTabBar::RightSide,tb);
    connect(tb,SIGNAL(clicked()),this,SLOT(incTab()));


    //view->setEnabled(true); 
    //view->page()->setNetworkAccessManager(m_manager);
    
    connect(view,SIGNAL(urlChanged(QUrl)),this,SLOT(updateUrl()));

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

    
    
    lineEdit1->show();
    urlLaunch->show();
    quit->show();
    view->show();
    
    frame->show();
    
    

}

MainWindow::~MainWindow(){
    free(views);
}

void MainWindow::incTab(){
    //account add tab button
    int index=tabAmount-1;

    if (tabAmount<=maxTabs){
    
    views[index]=new QWebView(tabControl);
    tabControl->addTab(views[index],QString::fromStdString(" "));
    tabAmount++;
    views[index]->setMouseTracking(true);
    views[index]->setStyleSheet("background:transparent");
    views[index]->setAttribute(Qt::WA_TranslucentBackground,true);
    views[index]->load(QUrl("http://www.google.com"));
    
    //views[index]->page()->setNetworkAccessManager(m_manager);
    //views[index]->show();
    connect(views[index],SIGNAL(urlChanged(QUrl)),this,SLOT(updateUrl()));
    //move the add tab button over 1
    tabControl->tabBar()->moveTab(tabAmount-2,tabAmount-1);
    
    //focus on new tab
    tabControl->setCurrentIndex(index); 
    }
    
}

void MainWindow::showTime()
{
    QTime time = QTime::currentTime();
    timeLabel->setText(time.toString());

}


void MainWindow::replyFinished(QNetworkReply *pReply){
    QByteArray data=pReply->readAll();
    
    /*
    QTextCodec *codec=QTextCodec::codecForMib(106);
    QString str = codec->toUnicode(data);
    QTextStream out(stdout);
    out << "fetch out" << str;
    */

    int titlestart=data.indexOf("<title>")+7;
    int titleend=data.indexOf("</title>");
    
    //convert to Unicode for foreign languages
    QString qpagetitle=QTextCodec::codecForMib(106)->
                       toUnicode(data.mid(titlestart,titleend-titlestart));
    

    //get webpage html title
    if(data.contains("<title>")){
        tabControl->setTabText(tabControl->currentIndex(),qpagetitle);
    }
}

void MainWindow::fetchUrl(std::string urlstr){
    QString qStrUrl = QString::fromStdString(urlstr);
    QUrl theurl = QUrl::fromUserInput(qStrUrl);
    m_manager->get(QNetworkRequest(theurl));

}

void MainWindow::updateUrlBar()
{
    int index = tabControl->currentIndex();
    std::cout << "Index change to " << index  <<"\n";
    
    QTextStream out(stdout);
    out << "Url is " << views[index]->url().toString() << "\n";
   
    if (views[index]->url().toString() != ""){
        lineEdit1->setText(views[index]->url().toString());

    }


}

void MainWindow::updateUrl()
{
//    QTextStream out(stdout);
//    out << "I am here";
    lineEdit1->setText(views[tabControl->currentIndex()]->url().toString());
    urlExists(views[tabControl->currentIndex()]->url());

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
bool MainWindow::urlExists(QUrl theurl){
    QTextStream out(stdout);
    QTcpSocket socket;
    QByteArray buffer;

        socket.connectToHost(theurl.host(), 80);
        if (socket.waitForConnected()) {
            //Standard http request
            socket.write("GET / HTTP/1.1\r\n"
            //socket.write("HEAD " + theurl.path().toUtf8() + " HTTP/1.1\r\n" 
                "host: " + theurl.host().toUtf8() + "\r\n\r\n");
            if (socket.waitForReadyRead()) {
                while(socket.bytesAvailable()){
                    buffer.append(socket.readAll());
                    //int packetSize = getPacketSize(buffer);
                    int packetSize=buffer.size();
                    while(packetSize>0)
                    {
                        //Output server response for debugging
                        out << "[" << buffer.data() << "]"; //<<endl;
                        
                       /* int titlestart=buffer.indexOf("<title>")+7;
                        int titleend=buffer.indexOf("</title>");
                        std::string pagetitle=QTextCodec::codecForMib(106)->
                            toUnicode(buffer.mid(titlestart,titleend-titlestart)).toStdString();

                        //get webpage html title
                        if(buffer.contains("<title>")){
                            tabControl->setTabText(0,QString::fromStdString(pagetitle));
                        }

                        */

                        //set Url if 200, 301, or 302 response given assuming that server will redirect
                        //Some pages such as play.google do not have an HTTP response...possibly
                        //differences in http protocol. Add exception for "Moved"
                        if (buffer.contains("200 OK") ||
                            buffer.contains("302 Found") ||
                            buffer.contains("301 Moved") ||
                            buffer.contains("Moved")
                            ) {
                            fetchUrl(lineEdit1->text().toStdString());
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

   if (urlstring.substr(0,7)!="http://" && urlstring.substr(0,8)!="https://"){
       std::string editstr;

       editstr="http://"+urlstring;
       QString qeditstr = QString::fromStdString(editstr);
       lineEdit1->setText(qeditstr); 
   
   }
   
   
   QUrl url = QUrl::fromUserInput(lineEdit1->text()); 
   if (urlExists(url)){
       std::cout<< "Current index is " << tabControl->currentIndex() << "\n";
       views[tabControl->currentIndex()]->load(url);}
       //view->setUrl(url);}
   else{
       qDebug() << QString("Invalid URL: %1").arg(url.toString());
       displayErrorHTML();    
       }
}

void MainWindow::displayErrorHTML(){
       view->load(QUrl::fromLocalFile(QFileInfo("urlinvalid.html").absoluteFilePath())); 
           //display error html page


}


void MainWindow::hideEvent(QHideEvent *event){
    event->accept();
}

void MainWindow::showEvent(QShowEvent *event){
    //QTextStream out(stdout);
     
    if(this->mainWinState==Max){
    //    out<<"I come out as Max";
    
        this->showMaximized();}
    else if(this->mainWinState==Min){
    //    out<<"I come out as Min";
        this->setWindowState(Qt::WindowNoState);
    }

    event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if( event->key() == Qt::Key_Return)
    {
        launchURL();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event){
   QTextStream out(stdout);
   QSize MWSize = this->size();

   //get current window dimensions
   int nheight = MWSize.height();
   int nwidth = MWSize.width();
   
    frame->resize(nwidth,nheight);

   //redraw border with round corners
    QBitmap bmp(size());
    bmp.clear();
    QPainter painter(&bmp);
    QPen pen(QColor(Qt::black));
    QBrush brush(QColor(Qt::black));
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRoundedRect(0,0,nwidth,nheight,20.0f,20.0f,Qt::AbsoluteSize);
    setMask(bmp);
   
   //Move clock
   timeLabel->move(nwidth-70,nheight-30);
    
   //Resize webview portion with window
   view->resize(nwidth,nheight-80);
   tabControl->resize(nwidth,nheight-80);

   //Resize url bar with window
   lineEdit1->resize(nwidth-200,20);

   //Move all buttons so they're in the right spot
   urlLaunch->move(nwidth-180,20);
   quit->move(nwidth-25,5);
   minmaxtoggle->move(nwidth-45,5);
   minimize->move(nwidth-65,5);
   
   event->accept();                
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

bool MainWindow::inResizeZone(QPoint rs_mpos){
    int rs_size=50;

    if (  //(this->width()>min_size && this->height()>min_size) && (
         (abs(rs_mpos.x()) < rs_size && abs(rs_mpos.y()) < rs_size) ||
         (abs(rs_mpos.x()) > this->width()-rs_size && abs(rs_mpos.y()) <rs_size) || 
         (abs(rs_mpos.x()) < rs_size && abs(rs_mpos.y())> this->height()-rs_size) ||
         (abs(rs_mpos.x()) > this->width()-rs_size && abs(rs_mpos.y())> this->height()-rs_size)
        // ) 
         ){
            return true;
         }

    return false;
}


void MainWindow::mouseMoveEvent(QMouseEvent *event){
   

    //mapped mouse relative to upper left of window
    rs_mpos=event->globalPos()-frameGeometry().topLeft();//general position tracker for resizing
    QTextStream out(stdout);
    //How much of the corner is considered a "resizing zone"
    //I was experiencing jumping behavior with rs_size is 10 so
    //I recommend rs_size=50
    
    //Modification Note: Resize zone of 50 is too large for cursor change.
    //Added new method inResizeZone(rs_mpos) where the resize zone is still 50
    //and a condition is added to window drag movement in this method.
    //However in this method resize zone is 30 so that mouse cursor change occurs at 30x30
    //but window drag movement is disabled at 50x50 to avoid jumping
    int rs_size=50;
    int min_size=100;
   
    //Big if statement checks if your mouse is in the upper left,
    //upper right, lower left, and lower right 
    
    /*if (  //(this->width()>min_size && this->height()>min_size) && (
         (abs(rs_mpos.x()) < rs_size && abs(rs_mpos.y()) < rs_size) ||
         (abs(rs_mpos.x()) > this->width()-rs_size && abs(rs_mpos.y()) <rs_size) || 
         (abs(rs_mpos.x()) < rs_size && abs(rs_mpos.y())> this->height()-rs_size) ||
         (abs(rs_mpos.x()) > this->width()-rs_size && abs(rs_mpos.y())> this->height()-rs_size)
        // ) 
         ){
    */
    
    if (inResizeZone(rs_mpos)){    
        
        //Below for debugging
        /*
        out << rs_mpos.x() << " , " << rs_mpos.y() << "\n";
        out << "window: " << this->width() << " , " << this->height() << "\n";
        out << "globalpos: " << event->globalPos().x() << " , " 
            << event->globalPos().y() << "\n";
        */

        //Use 2x2 matrix to adjust how much you are resizing and how much you
        //are moving. Since the default coordinates are relative to upper left
        //You cannot just have one way of resizing and moving the window.
        //It will depend on which corner you are referring to

        //adjXfac and adjYfac are for calculating the difference between your
        //current mouse position and where your mouse was when you clicked.
        //With respect to the upper left corner, moving your mouse to the right
        //is an increase in coordinates, moving mouse to the bottom is increase
        //etc.
        //However, with other corners this is not so and since I chose to subtract
        //This difference at the end for resizing, adjXfac and adjYfac should be
        //1 or -1 depending on whether moving the mouse in the x or y directions
        //increases or decreases the coordinates respectively. 

        //transXfac transYfac is to move the window over. Resizing the window does not
        //automatically pull the window back toward your mouse. This is what
        //transfac is for (translate window in some direction). It will be either
        //0 or 1 depending on whether you need to translate in that direction.

        //Initiate matrix
        int adjXfac=0;
        int adjYfac=0;
        int transXfac=0;
        int transYfac=0;

        //Upper left corner section
        if ( (abs(rs_mpos.x()) < rs_size && abs(rs_mpos.y()) < rs_size)){
            this->setCursor(Qt::SizeFDiagCursor);
    
            
            //Upper left. No flipping of axis, no translating window
               adjXfac=1;
               adjYfac=1;
                
               transXfac=0;
               transYfac=0;


        }
        //Upper right corner section
        else if(abs(rs_mpos.x()) > this->width()-rs_size && abs(rs_mpos.y()) <rs_size){
            this->setCursor(Qt::SizeBDiagCursor);
    
    
            //upper right. Flip displacements in mouse movement across x axis
            //and translate window left toward the mouse
            adjXfac=-1;
            adjYfac=1;

            transXfac = 1;
            transYfac =0;    


        }
        //Lower left corner section
        else if(abs(rs_mpos.x()) < rs_size && abs(rs_mpos.y())> this->height()-rs_size){
            this->setCursor(Qt::SizeBDiagCursor);
                
            //lower left. Flip displacements in mouse movement across y axis
            //and translate window up toward mouse
            adjXfac=1;
            adjYfac=-1;

            transXfac=0;
            transYfac=1;
            

        }   
        //Lower right corner section
        else if(abs(rs_mpos.x()) > this->width()-rs_size && abs(rs_mpos.y())> this->height()-rs_size){
            this->setCursor(Qt::SizeFDiagCursor);
            
            //lower right. Flip mouse displacements on both axis and
            //translate in both x and y direction left and up toward mouse.
            adjXfac=-1;
            adjYfac=-1;

            transXfac=1;
            transYfac=1;
        }
       

        if (event->buttons()==Qt::LeftButton){
              
           
           //Calculation of displacement. adjXfac=1 means normal displacement
           //adjXfac=-1 means flip over axis     
           int adjXdiff = adjXfac*(event->globalPos().x() - global_mpos.x());
                
           int adjYdiff = adjYfac*(event->globalPos().y() - global_mpos.y());
           
           //if transfac is 1 then movepoint of mouse is translated     
           QPoint movePoint(mpos.x() - transXfac*adjXdiff, mpos.y()-transYfac*adjYdiff);
            
            if (storeWidth-adjXdiff>min_size && storeHeight-adjYdiff>min_size){
                move(event->globalPos()-movePoint);
           
                resize(storeWidth-adjXdiff, storeHeight-adjYdiff);
            }     
                event->accept();
                
                
        }

    }


    //in any move event if it is not in a resize region use the default cursor
    //Move window
    else{
    
        this->setCursor(Qt::ArrowCursor);
        //simple move section
        if (event->buttons()==Qt::LeftButton){
            move(event->globalPos() - mpos);
            event->accept();
        }
    }
    

    

}
