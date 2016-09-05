#include "MainWindow.h"
#include "clefia.h"
#include <math.h>
#include <QApplication>
#include <QtDebug>
#include <QElapsedTimer>

qint64 testClefia(QString data);

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

//    int i;
//    qint64 time, rata2;
//    int result = 0;
//    QString test = "Seminar Nasional Teknologi Informasi dan Aplikasinya";
//    for (i = 0; i < 10; ++i) {
//        time = testClefia(test);
//        result += time;
//    }
//    rata2 = result/10;
//    qDebug() << rata2 << "ns";

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

qint64 testClefia(QString data)
{
    Clefia clefia;
    MainWindow main;
    QElapsedTimer enctime, decTime;
    qint64 encTimer = 0, decTimer = 0;

    // Untuk pengujian :
    const unsigned char skey[32] = {
        0xffU,0xeeU,0xddU,0xccU,0xbbU,0xaaU,0x99U,0x88U,
        0x77U,0x66U,0x55U,0x44U,0x33U,0x22U,0x11U,0x00U,
        0xf0U,0xe0U,0xd0U,0xc0U,0xb0U,0xa0U,0x90U,0x80U,
        0x70U,0x60U,0x50U,0x40U,0x30U,0x20U,0x10U,0x00U
    };

//    QString data = "Another kind of formatter enables you to use a character mask "
//                   "to specify the set of characters that can be typed at each position in the field.";

    unsigned char rk[8 * 26 + 16];

    int blockClefia     = 128;
    int lenData         = data.size();
    int lenClefia       = 16;
    int lengthOf        = ceil(((float)lenData/lenClefia));

    QString temp[lengthOf];
    QString tempe[lengthOf];

    unsigned char tempo[lengthOf*lenClefia];
    unsigned char tempi[lengthOf*lenClefia];

    unsigned char datax[lenClefia], ct[lenClefia], dst[lenClefia];
    int r = clefia.ClefiaKeySet(rk, skey, blockClefia);

    enctime.start();
    for (int i = 0; i < lengthOf; ++i) {
        for (int j = 0; j < lenClefia; ++j) {
            temp[i] += data[i*lenClefia + j];
        }
        memcpy(datax, temp[i].toStdString().c_str(), lenClefia);
//        printf("+++++++++++++++++++Testing+++++++++++++++++++++\n");
//        printf("datax       : "); clefia.BytePut(datax, lenClefia);
        clefia.ClefiaEncrypt(dst, datax, rk, r);
//        printf("ciphertext  : ");
//        clefia.BytePut(dst, lenClefia);

        for (int j = 0; j < lenClefia; ++j) {
            tempo[i*lenClefia + j] = dst[j];
        }
    }

    encTimer = enctime.nsecsElapsed();
//    qDebug() << "Encryption Time elapsed: " << encTimer << "nS";

//    for (int i = 0; i < lengthOf*lenClefia; ++i) {
//        printf("%.2x",tempo[i]);
//    }

    QString msg = main.getStringFromUnsignedChar(tempo,sizeof(tempo));
//    qDebug() << "+++++++++++++++++++Sending+++++++++++++++++++++";
//    qDebug() << "message:" << data;
    qDebug() << "cipher :" << msg.size();

    printf("\n");

    decTime.start();
    for (int i = 0; i < lengthOf; ++i) {
        for (int j = 0; j < lenClefia; ++j) {
            tempe[i] += msg[i*lenClefia + j];
        }
        unsigned char* datay = (unsigned char*)tempe[i].toLatin1().data();

        clefia.ClefiaDecrypt(ct, datay, rk, r);
//        printf("datay       : "); clefia.BytePut(datay, lenClefia);
//        printf("plaintext   : ");
//        clefia.BytePut(ct, lenClefia);
//        printf("+++++++++++++++++++++End+++++++++++++++++++++++\n");

        for (int j = 0; j < lenClefia; ++j) {
            tempi[i*lenClefia + j] = ct[j];
        }
    }

    decTimer = decTime.nsecsElapsed();
//    qDebug() << "Decryption Time elapsed: " << decTimer << "nS";

//    for (int i = 0; i < lengthOf*lenClefia; ++i) {
//        printf("%.2x",tempi[i]);
//    }

    QByteArray dataa = QByteArray(reinterpret_cast<char*>(tempi), lenData);
//    qDebug() << "plain  :" << dataa;

    return encTimer;
}
