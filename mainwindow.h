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
        ~MainWindow();
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
        void updateUrlBar(int index);
        void replyFinished(QNetworkReply *pReply);

        //timer
        void showTime();

        //increment tab
        void incTab();

        //close tab
        void closeMyTab(int index);
        
        //rearrange indices when tab moves
        void tabIndexRearrange(int from, int to);

    protected:  
        //Mouse
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;


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
        bool resizeLock; //if resizeLock (left mouse press within resize zone) then resize anywhere
        int adjXfac_lock;
        int adjYfac_lock;
        int transXfac_lock;
        int transYfac_lock;

        //Resize method
        bool inResizeZone(QPoint rs_mpos);

        //Misc 
        std::string pageTitle; //Get webpage html title
        
        //General controls
        QWebView *view;
        QLineEdit *lineEdit1;
        QPushButton *urlLaunch;
        QPushButton *quit;
        QPushButton *minmaxtoggle;
        QPushButton *minimize;
        QTabWidget *tabControl;
        QFrame *frame;
        QNetworkAccessManager *m_manager; 
        QLabel *timeLabel;

        QSignalMapper *signalMapper;
        
        //Structure of tab related things
        struct Tabs{
            QWebView *webView;
            QToolButton *webViewClose;
            int index;
            bool initialized;
        };

        Tabs **tabs;
        //methods
        void fetchUrl(std::string urlstr);
        bool urlExists(QUrl theurl);
    signals:
        void clicked(int index);
};

