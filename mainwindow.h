#define WIN_X_SIZE 1000
#define WIN_Y_SIZE 600
#include <string>
#include <QMainWindow>
#include <QPushButton>
#include <QtWebKit>
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
        void launchURL();
        void WinMinimize();
        void WinMinMaxToggle();
        void showEvent(QShowEvent * event);
        void hideEvent(QHideEvent * event);
        void keyPressEvent(QKeyEvent *event);
        void resizeEvent(QResizeEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void updateUrl();
        bool urlExists(QUrl theurl);
    protected:  
    private:
        enum WindowState {Min, Max, Full};
        WindowState mainWinState;
        QPoint mpos; //For dragging, relative mouse position to upper left
        QPoint mpos_2;
        QPoint global_mpos; //For resizing, global mouse position at mouse click
        QPoint rs_mpos; //for resizing
        QPoint rs_global_mpos;
        int storeWidth; //fix window size at mouseclick for resizing
        int storeHeight;
        std::string pageTitle; //Get webpage html title
        QWebView *view;
        QLineEdit *lineEdit1;
        QPushButton *urlLaunch;
        QPushButton *quit;
        QPushButton *minmaxtoggle;
        QPushButton *minimize;
        QTabWidget *tabControl;
            
};

