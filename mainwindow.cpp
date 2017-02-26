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
    resizeLock=false;


    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)),
            this, SIGNAL(clicked(int)));
    


    //Main window
    this->resize(WIN_X_SIZE,WIN_Y_SIZE);
    this->setWindowTitle("HARbrowse");
    this->setToolTip("This is a test");
    this->mainWinState = Min;
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

    //Needed to get webpage title
    m_manager=new QNetworkAccessManager(this);
    connect(m_manager,SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    


    //Tab Page section
    tabControl = new QTabWidget(this);
    tabControl->setMovable(true);
    tabControl->setMouseTracking(true);
    tabControl->resize(1000,520);
    tabControl->move(0,50);
    tabControl->setAttribute(Qt::WA_TranslucentBackground,true);
    //tabControl->setStyleSheet("background:transparent");
    
    connect(tabControl,SIGNAL(currentChanged(int)),this, SLOT(updateUrlBar(int)));
    connect(tabControl->tabBar(),SIGNAL(tabMoved(int,int)),
            this,SLOT(tabIndexRearrange(int,int)));


    //Webview
    view=new QWebView(tabControl);

    QTextStream out(stdout);
    //backwards compatability to not have to redo all the code that says "view"
    Tabs* firstTab=new Tabs();
    firstTab->webView=view;
    
    
    tabList = new TabList();
    tabList->tabsHead=firstTab;

    Tabs* firstHeadTab = tabList->getHeadTab();
    firstHeadTab->index=0;
    firstHeadTab->setNext(NULL);
    firstHeadTab->setPrev(NULL);


    tabControl->addTab(view,QString::fromStdString(" "));
    firstHeadTab->initialized=true;
    
    //tabControl->setTabsClosable(true);
    firstHeadTab->webViewClose = new QToolButton(this);
    firstHeadTab->webViewClose->setText("x");
    connect(firstHeadTab->webViewClose,SIGNAL(clicked()),signalMapper,SLOT(map()));
    signalMapper->setMapping(firstHeadTab->webViewClose,0);
    tabControl->tabBar()->setTabButton(0,QTabBar::RightSide,firstHeadTab->webViewClose);

    view->setMouseTracking(true);
    view->setStyleSheet("background:transparent");
    view->setAttribute(Qt::WA_TranslucentBackground,true);
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled,true);    
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages,true);    
    view->load(QUrl("http://www.google.com"));


    //Add tab button. The tab itself is disabled so that the user will not
    //focus on it when clicking
    QToolButton *tb = new QToolButton(this);
    tb->setText("+");
    tabControl->addTab(new QLabel("Add tabs by pressing +"),QString());
    tabControl->setTabEnabled(1,false);
    tabControl->tabBar()->setTabButton(1,QTabBar::RightSide,tb);
    connect(tb,SIGNAL(clicked()),this,SLOT(incTab()));

    //Click to close
    connect(this, SIGNAL(clicked(int)), this, SLOT(closeMyTab(int)));

    
    connect(view,SIGNAL(urlChanged(QUrl)),this,SLOT(updateUrl()));

    //Back and forward buttons
    //QIcon backIcon= style->standardIcon(QStyle::SP_ArrowBack);
    webBack=new QPushButton(this);
    //webBack->setIcon(backIcon);
    webBack->setText("<<");
    webBack->resize(20,20);
    webBack->move(15,20);
    webBack->setAutoDefault(false);
    webBack->setDefault(false);
    connect(webBack,SIGNAL(clicked()),this,SLOT(webViewBack()));

    webForward=new QPushButton(this);
    webForward->setText(">>");
    webForward->resize(20,20);
    webForward->move(40,20); 
    webForward->setAutoDefault(false);
    webForward->setDefault(false);
    connect(webForward,SIGNAL(clicked()),this,SLOT(webViewForward()));

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
    lineEdit1->resize(740,20);
    lineEdit1->move(60,20);
    lineEdit1->setStyleSheet(".QLineEdit{border-radius: 10px;}");

    //Launch URL section
    urlLaunch = new QPushButton(this);
    urlLaunch->setText("Launch");
    urlLaunch->resize(70,20);
    urlLaunch->move(800,20);
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
    webBack->show();
    webForward->show();    
    frame->show();
    
    

}

MainWindow::~MainWindow(){
    delete tabList;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    resizeLock=false;
    event->accept();
}

void MainWindow::deleteTabPointer(Tabs* tabPointer){
       delete tabPointer->webViewClose;
       delete tabPointer->webView;
       delete tabPointer;
}


void MainWindow::closeMyTab(int index){

   QTextStream out(stdout);
   out<< "Index being closed is " << index;
   Tabs *tmp = tabList->getTab(index);

   if (tmp==NULL){
       return;

   }
   tabControl->removeTab(index);
   
   if (tmp->getPrev()==NULL && tmp->getNext()==NULL){
       //It's the head node 
       deleteTabPointer(tmp); 
       tabList->setHead(NULL);
       tabConnectionCheck();
       return;
   }
    

   //Set next of previous node to next of current
   Tabs* prevNode=tmp->getPrev();
   Tabs* nextNode=tmp->getNext();
   
   if (nextNode==NULL){
       //It's the end tab, delete it and set prevNode next to NULL
       prevNode->setNext(NULL);
       
       //Move focus to tab before end tab to prevent focus from
       //going onto add tab tab
       tabControl->setCurrentIndex(index-1);
       deleteTabPointer(tmp); 
       tabConnectionCheck();
       return;
   }
   
   //What do we do about head node where prev is Null?
   if (prevNode==NULL){
         //If prev is NULL then we're deleting the head tab but there are other nodes
         //attached so set head of list to next of tab being deleted
         tabList->setHead(nextNode);
         nextNode->setPrev(NULL);
   }
   else{
   
        prevNode->setNext(nextNode);
        nextNode->setPrev(prevNode);

   }
   deleteTabPointer(tmp); 
   
   if(tmp!=NULL){
        out<< "\ntab deletion did not succeed!"<<endl;
        if (tmp->webView !=NULL){
            out << "webView exists";
        }
        if (tmp->webViewClose != NULL){
            out<< "webView Close exists";
        }
   }

   //update indices
   while (nextNode!=NULL){
        nextNode->index=nextNode->index-1;
        signalMapper->setMapping(nextNode->webViewClose,nextNode->index);
        nextNode=nextNode->getNext();
   }
    
   //update current url bar
   updateUrlBar(tabControl->currentIndex());


   tabConnectionCheck();



}

void MainWindow::tabConnectionCheck(){
   QTextStream out(stdout);
   out << "\nStart connection check" << endl;
   for(int i=0; i<=tabList->getLastTabIndex();i++){
        Tabs* theTab=tabList->getTab(i);
        out << "Tab " << theTab->index << " is connected to: " << endl;
        
        if (theTab->getPrev() != NULL) {
            out << "\t"<<theTab->getPrev()->index << " back" << endl;
        }
        else{
            out << "\t"<<"Null back"<<endl;
        }
        
        if (theTab->getNext() != NULL) {
            out << "\t"<<theTab->getNext()->index << " forward" << endl;
        }
        else{
            out << "\t"<<"Null forward"<<endl;
        }

   }

}

void MainWindow::tabIndexRearrange(int from, int to){
    
    QTextStream out(stdout);
    
    int lastTabIndex = tabList->getLastTabIndex();
    
    //For the add tab tab nothing should be done here
    if (from > lastTabIndex or to >lastTabIndex){
        return;
    }

    Tabs* fromTab;
    Tabs* fromNext;
    Tabs* fromPrev;

    Tabs* toTab;
    Tabs* toNext;
    Tabs* toPrev;
    
    fromTab = tabList->getTab(from);
    fromNext = fromTab->getNext();
    fromPrev = fromTab->getPrev();
    
    toTab=tabList->getTab(to);
    toNext= toTab->getNext();
    toPrev= toTab->getPrev();

    fromTab->setIndex(to);
    toTab->setIndex(from);

    //Right to left movement
    if (from>to){
        /*
         Assuming ...Node1->Node2->Node3->Null
         Where 3 goes to 2
         Steps:
            1. 3 next is what 2 is
            2. 3 prev is what 2's prev was
            3. 2 prev is what 3 is
            4. 2 next is what 3's next was
            5. 1 next is 3

        */
        fromTab->setNext(toTab);
        fromTab->setPrev(toPrev);

        toTab->setPrev(fromTab);
        toTab->setNext(fromNext);

        toPrev->setNext(fromTab);
    }
    
    //left to right movement
    if (from<to){
        /*
         Assuming ...Node1->Node2->Node3->Null
         Where 2 goes to 3
         Steps:
            1. 2 prev is what 3 is
            2. 2 next is what 3's next was
            3. 3 next is what 2 is
            4. 3 prev is what 2's prev was
            5. 1 next is 3

        */
        fromTab->setPrev(toTab);
        fromTab->setNext(toNext);

        toTab->setNext(fromTab);
        toTab->setPrev(fromPrev);

        fromPrev->setNext(toTab);
    }
    
}


int MainWindow::TabList::getLastTabIndex(){
    Tabs *temp=getHeadTab();    
    int lastIndex=-1;
    while (temp != NULL){
        temp=temp->getNext();
        lastIndex++;
    }  

    return lastIndex;

}

MainWindow::Tabs* MainWindow::TabList::getTab(int findIndex){
    Tabs *temp;//=new Tabs();
    temp=tabsHead;
    bool notfound=true;
     
    QTextStream out(stdout);
    if (temp != NULL){
        while (notfound && temp != NULL){
            if (temp->index == findIndex){

                notfound=false;

            }
            else{
                temp=temp->getNext();
            }

        }
    }

    return temp;
}

MainWindow::Tabs* MainWindow::TabList::getLastTab(){
    
    if (getLastTabIndex()>=0){
        return getTab(getLastTabIndex());
    }

    return NULL;

}


void MainWindow::incTab(){

    
    QTextStream out(stdout);
    
    int lastIndex = tabList->getLastTabIndex();

    Tabs* lastTab = tabList->getLastTab();

    //make new tab, give it the last tab's index + 1
    //set the new tab's next tab to NULL since it will be
    //appended to the end of the tab list
    //New tab's prevTab will be the lasttab on the current list    
    Tabs* newTab = new Tabs();
    newTab->setIndex(lastIndex+1);
    newTab->webView=new QWebView(tabControl);
    newTab->setNext(NULL);
    newTab->setPrev(lastTab);

    //Link last tab on tab list to new tab if it isn't head tab
    if (lastTab!=NULL){
        lastTab->setNext(newTab);
    }
    else {
        tabList->setHead(newTab);
    }

    
    //From here the last tab has chaned
    Tabs* newLastTab = tabList->getLastTab();
    int newLastTabIndex=tabList->getLastTabIndex();

    tabControl->addTab(newLastTab->webView,QString::fromStdString(" "));
    
    //move the add tab button over 1 because the new tab was actually added to the end
    //and the add tab tab counts as a tab (add tab is at newLastTabIndex before below
    //statement executed)
    tabControl->tabBar()->moveTab(newLastTabIndex,newLastTabIndex+1);
    
    
    //tab settings
    
    newLastTab->webView->setMouseTracking(true);
    newLastTab->webView->setStyleSheet("background:transparent");
    newLastTab->webView->setAttribute(Qt::WA_TranslucentBackground,true);
    newLastTab->webView->load(QUrl("http://www.google.com"));
   
    //button to close the tab

    newLastTab->webViewClose=new QToolButton(this);
    newLastTab->webViewClose->setText("x");
    
    //Map the index number to the index of the Tabs struct
    //so that the right tab can be closed when its button its
    
    connect(newLastTab->webViewClose,SIGNAL(clicked()),
            signalMapper,SLOT (map()));
    signalMapper->setMapping(newLastTab->webViewClose,newLastTabIndex);

    tabControl->tabBar()->setTabButton(newLastTabIndex,QTabBar::RightSide,newLastTab->webViewClose);
    
    //Connect url manipulation of new tab's webview
    connect(newLastTab->webView,SIGNAL(urlChanged(QUrl)),this,SLOT(updateUrl()));
    
    
    //focus on new tab
    tabControl->setCurrentIndex(newLastTabIndex);
    
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

void MainWindow::updateUrlBar(int index)
{
    QTextStream out(stdout);

    //If it's focused on the add tab tab then
    //Focus should go to last actual tab instead
    //Probably should be implemented somewhere else but for now 
    //Leave it here
    if (index>tabList->getLastTabIndex()){
        tabControl->setCurrentIndex(tabList->getLastTabIndex());
        return;
    }
    

    if (tabList->getTab(index)->webView->url().toString() != ""){
        lineEdit1->setText(tabList->getTab(index)->webView->url().toString());

    }
}

void MainWindow::updateUrl()
{
    
    lineEdit1->setText(tabList->getTab(tabControl->currentIndex())->webView->url().toString());
    urlExists(tabList->getTab(tabControl->currentIndex())->webView->url());

}

void MainWindow::webViewBack(){

    int index=tabControl->currentIndex();
    tabList->getTab(index)->webView->back();
}

void MainWindow::webViewForward(){
    int index=tabControl->currentIndex();
    tabList->getTab(index)->webView->forward();
    

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
                        //out << "[" << buffer.data() << "]"; //<<endl;
                        
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

   if (urlstring.substr(0,7)!="http://" && urlstring.substr(0,8)!="https://"){
       std::string editstr;

       editstr="http://"+urlstring;
       QString qeditstr = QString::fromStdString(editstr);
       lineEdit1->setText(qeditstr); 
   
   }
   
   
   QUrl url = QUrl::fromUserInput(lineEdit1->text()); 
   if (urlExists(url)){
       //QTextStream out(stdout);
       
       tabList->getTab(tabControl->currentIndex())->webView->load(url);
       
       }
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
   lineEdit1->resize(nwidth-240,20);

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
        
        if (!resizeLock){
        
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
            //in any move event if it is not in a resize region use the default cursor
            else{
        
                this->setCursor(Qt::ArrowCursor);
            }

       }

    if (inResizeZone(rs_mpos) && 
        event->buttons()==Qt::LeftButton &&
        resizeLock==false){
        
        resizeLock=true;
        adjXfac_lock=adjXfac;
        adjYfac_lock=adjYfac;

        transXfac_lock=transXfac;
        transYfac_lock=transYfac;

    }
    
    //if (inResizeZone(rs_mpos)){    
    
    //resize if resizeLock true
    if (resizeLock){    
        
       

        if (event->buttons()==Qt::LeftButton){
              
           
           //Calculation of displacement. adjXfac_lock=1 means normal displacement
           //adjXfac_lock=-1 means flip over axis     
           int adjXdiff = adjXfac_lock*(event->globalPos().x() - global_mpos.x());
                
           int adjYdiff = adjYfac_lock*(event->globalPos().y() - global_mpos.y());
           
           //if transfac_lock is 1 then movepoint of mouse is translated     
           QPoint movePoint(mpos.x() - transXfac_lock*adjXdiff, mpos.y()-transYfac_lock*adjYdiff);
            
            if (storeWidth-adjXdiff>min_size && storeHeight-adjYdiff>min_size){
                move(event->globalPos()-movePoint);
           
                resize(storeWidth-adjXdiff, storeHeight-adjYdiff);
            }     
                event->accept();
                
                
        }

    }


    else{ 
        //simple move section
        if (event->buttons()==Qt::LeftButton){
            move(event->globalPos() - mpos);
            event->accept();
        }
    }
    

}

