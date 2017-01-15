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
    protected:  
    private:
        QWebView *view;
        QLineEdit *lineEdit1;
        //void launchUrl();

};

