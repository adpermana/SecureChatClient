#include <QMainWindow>
#include <QTcpSocket> 
#include <math.h>
#include "ui_MainWindow.h"
#include "simplecrypt.h"
#include "clefia.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent=0);

private slots:
    void on_loginButton_clicked();
    void on_sayButton_clicked();
    void readyRead();
    void connected();

public slots:
    QString getStringFromUnsignedChar(unsigned char *str, int len);

private:
    QTcpSocket *socket;
    SimpleCrypt crypto;
    Clefia clefia;
    unsigned char rk[8 * 26 + 16];
    const unsigned char skey[32] = {
        0xffU,0xeeU,0xddU,0xccU,0xbbU,0xaaU,0x99U,0x88U,
        0x77U,0x66U,0x55U,0x44U,0x33U,0x22U,0x11U,0x00U,
        0xf0U,0xe0U,0xd0U,0xc0U,0xb0U,0xa0U,0x90U,0x80U,
        0x70U,0x60U,0x50U,0x40U,0x30U,0x20U,0x10U,0x00U
    };
    int count;
    int blockClefia     = 128;
    int lenClefia       = 16;
    int tempoLen;
    QString clefiaEncryptQString(QString message);
    QByteArray clefiaDecryptQString(QString message);
};
