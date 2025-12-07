#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QBluetoothServer>
#include <QBluetoothSocket>

class StreamReader;

class Translator : QObject
{
    Q_OBJECT

public:
    Translator();
    ~Translator();

private:
    StreamReader *m_pStreamReader;
    QBluetoothServer *m_pServer;
    QBluetoothSocket *m_pSocket;
    bool m_bAHRS;
    bool m_bGPS;
    bool m_bStratux;
    bool m_bSendStream;
    QDateTime m_lastStatusUpdate;
    int m_iRoutineTimer;

    QByteArray buildGDL90(byte id, QByteArray data);
    unsigned short crc16(const char* data, int length);
    void sendDummyHeartbeat();

private slots:
    void status( bool bStratux, bool bAHRS, bool bGPS );
    void timerEvent( QTimerEvent *pEvent );
    void situation( StratuxSituation s );
    void traffic( int iICAO, StratuxTraffic t );
    void newConnection();
    void readyRead();
    void disconnected();
};

#endif // TRANSLATOR_H