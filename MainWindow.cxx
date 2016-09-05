#include "MainWindow.h"
#include <QRegExp>
#include <QBuffer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    stackedWidget->setCurrentWidget(loginPage);
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));

    crypto.setKey(0x0c2ad4a4acb9f023);

    count = clefia.ClefiaKeySet(rk, skey, blockClefia);
}

void MainWindow::on_loginButton_clicked()
{
    socket->connectToHost(serverLineEdit->text(), 4200);
}

void MainWindow::connected()
{
    stackedWidget->setCurrentWidget(chatPage);

    socket->write(QString("/me:" + userLineEdit->text() + "\n").toUtf8());
}

QString MainWindow::getStringFromUnsignedChar(unsigned char *str, int len)
{
    QString s, result = "";

    for ( int i = 0; i<len; i++){
        s = (QChar(str[i]));
        if(s == "<\>"){
            s="0";
        }
        result.append(s);
    }
    return result;
}

QString MainWindow::clefiaEncryptQString(QString message)
{
    int lenData         = message.size();
    int lengthOf        = ceil(((float)lenData/lenClefia));

    unsigned char datax[lenClefia], dst[lenClefia];
    unsigned char tempo[lengthOf*lenClefia];
    QString temp[lengthOf];

    for (int i = 0; i < lengthOf; ++i) {
        for (int j = 0; j < lenClefia; ++j) {
            temp[i] += message[i*lenClefia + j];
        }

        memcpy(datax, temp[i].toStdString().c_str(), lenClefia);
        clefia.ClefiaEncrypt(dst, datax, rk, count);

        for (int j = 0; j < lenClefia; ++j) {
            tempo[i*lenClefia + j] = dst[j];
        }
    }
    QString msg = getStringFromUnsignedChar(tempo,sizeof(tempo));
    return msg;
}

QByteArray MainWindow::clefiaDecryptQString(QString message)
{
    int lenData         = message.size();
    int lengthOf        = ceil(((float)lenData/lenClefia));

    unsigned char ct[lenClefia];
    unsigned char tempi[lengthOf*lenClefia];
    QString tempe[lengthOf];
    qDebug() << "messageRcv: " << message;

    for (int i = 0; i < lengthOf; ++i) {
        for (int j = 0; j < lenClefia; ++j) {
            tempe[i] += message[i*lenClefia + j];
        }
        unsigned char* datay = (unsigned char*)tempe[i].toLatin1().data();
        clefia.ClefiaDecrypt(ct, datay, rk, count);

        for (int j = 0; j < lenClefia; ++j) {
            tempi[i*lenClefia + j] = ct[j];
        }
    }
    QByteArray msg = QByteArray(reinterpret_cast<char*>(tempi), sizeof(tempi));
    return msg;
}

void MainWindow::on_sayButton_clicked()
{
    QString message = sayLineEdit->text().trimmed();
    QString msg     = clefiaEncryptQString(message);

    if(!message.isEmpty())
    {
        socket->write(QString(msg + "\n").toUtf8());
    }
    sayLineEdit->clear();
    sayLineEdit->setFocus();
}

void MainWindow::readyRead()
{
    while(socket->canReadLine())
    {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        QRegExp messageRegex("^([^:]+):(.*)$");
        QRegExp usersRegex("^/users:(.*)$");

        if(usersRegex.indexIn(line) != -1)
        {
            QStringList users = usersRegex.cap(1).split(",");
            userListWidget->clear();
            foreach(QString user, users)
                new QListWidgetItem(QPixmap(":/user.png"), user, userListWidget);
        }
        else if(messageRegex.indexIn(line) != -1)
        {
            QString user    = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            if (user != "Server") {
                QByteArray msg = clefiaDecryptQString(message);
                roomTextEdit->append("<b>" + user + "</b>: " + msg);

            }else if (user == "Server") {
                roomTextEdit->append("<b>" + user + "</b>: " + message);
            }
        }
    }
}
