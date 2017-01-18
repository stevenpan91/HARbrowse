#define WIN_X_SIZE 1000
#define WIN_Y_SIZE 600

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
    private slots:
        void launchURL();
        void WinMinimize();
        void keyPressEvent(QKeyEvent *event);
        void resizeEvent(QResizeEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
    protected:  
    private:
        QPoint mpos;
        QWebView *view;
        QLineEdit *lineEdit1;
        QPushButton *urlLaunch;
        QPushButton *quit;
        QPushButton *minimize;

};

