#define WIN_X_SIZE 1000
#define WIN_Y_SIZE 600
#include <string>
#include <QMainWindow>
#include <QPushButton>
#include <QtWebKit>
#include <QtWebKitWidgets>
#include <QLabel>
#include <QLineEdit>
namespace Ui{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent=0);
        void displayErrorHTML();
    private slots:
        //window display style related
        void WinMinimize();
        void WinMinMaxToggle();
        void showEvent(QShowEvent * event);
        void hideEvent(QHideEvent * event);
        
        void keyPressEvent(QKeyEvent *event);
        
       
        //related to browsing internet 
        void launchURL();
        void updateUrl();
        void replyFinished(QNetworkReply *pReply);

        //timer
        void showTime();

        //increment tab
        void incTab();
    protected:  
        //Mouse
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        
        //window display
        void resizeEvent(QResizeEvent *event) override;
    private:
        //Window state
        enum WindowState {Min, Max, Full};
        WindowState mainWinState;
       
        //Maximum number of tabs
        int maxTabs;
        int tabAmount;

        //Resizing parameters
        QPoint mpos; //For dragging, relative mouse position to upper left
        QPoint mpos_2;
        QPoint global_mpos; //For resizing, global mouse position at mouse click
        QPoint rs_mpos; //for resizing
        QPoint rs_global_mpos;
        int storeWidth; //fix window size at mouseclick for resizing
        int storeHeight;
       
        //Resize method
        bool inResizeZone(QPoint rs_mpos);

        //Misc 
        std::string pageTitle; //Get webpage html title
        
        //General controls
        //std::vector<QWebView> views;
        QWebView **views;
        QWebView *view;
        QLineEdit *lineEdit1;
        QPushButton *urlLaunch;
        QPushButton *quit;
        QPushButton *minmaxtoggle;
        QPushButton *minimize;
        QTabWidget *tabControl;
        QFrame *frame;
        QNetworkAccessManager *m_manager; 
        QNetworkCookieJar *cookieJar;
        QLabel *timeLabel;

        //methods
        void fetchUrl(std::string urlstr);
        bool urlExists(QUrl theurl);
};

