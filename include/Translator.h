#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QTimer>
#include <QDateTime>
#include "StubTypes.h"

class StreamReader;

class Translator : public QObject
{
    Q_OBJECT

public:
    explicit Translator();
    ~Translator();

private slots:
    // Slots for StreamReader (Currently unused stubs)
    void situation(StratuxSituation s);
    void traffic(int iICAO, StratuxTraffic t);
    void status(bool bStratux, bool bAHRS, bool bGPS);

    // Bluetooth slots
    void newConnection();
    void socketDisconnected();
    void readyRead();

protected:
    void timerEvent(QTimerEvent *pEvent) override;

private:
    void sendDummyHeartbeat();
    QByteArray buildGDL90(quint8 msgId, const QByteArray &payload);
    quint16 crc16(const char *data, int len);

    StreamReader     *m_pStreamReader;
    QBluetoothServer *m_pServer;
    QBluetoothSocket *m_pSocket;

    int               m_iRoutineTimer;
    bool              m_bSendStream;
};

#endif // TRANSLATOR_H