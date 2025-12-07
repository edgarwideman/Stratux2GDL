#include "Translator.h"
#include "StreamReader.h"
#include <QDebug>
#include <QBluetoothLocalDevice>

Translator::Translator()
    : QObject(nullptr),
      m_pStreamReader(new StreamReader(this)),
      m_pServer(new QBluetoothServer(QBluetoothUuid::SerialPort, this)),
      m_pSocket(nullptr),
      m_bSendStream(false)
{
    // Mock connections (won't fire in this version, but ready for later)
    connect(m_pStreamReader, &StreamReader::newSituation, this, &Translator::situation);
    connect(m_pStreamReader, &StreamReader::newStatus, this, &Translator::status);
    connect(m_pStreamReader, &StreamReader::newTraffic, this, &Translator::traffic);

    // Start Bluetooth Server
    connect(m_pServer, &QBluetoothServer::newConnection, this, &Translator::newConnection);
    
    // Listen on SPP (Serial Port Profile)
    bool listening = m_pServer->listen(QBluetoothUuid::SerialPort);
    
    if (listening) {
        qDebug() << "Bluetooth Listening as GDL39 on Channel" << m_pServer->serverPort();
    } else {
        qDebug() << "Failed to start Bluetooth listener. Check hardware/permissions.";
    }

    // 1Hz Timer for Heartbeat
    m_iRoutineTimer = startTimer(1000);
}

Translator::~Translator()
{
    if (m_pSocket) {
        m_pSocket->disconnectFromService();
        m_pSocket->deleteLater();
    }
    delete m_pStreamReader;
}

void Translator::newConnection()
{
    // If we already have a connection, ignore new ones or drop old one.
    // For simplicity, we take the new one.
    if (m_pSocket) {
        m_pSocket->disconnectFromService();
        m_pSocket->deleteLater();
    }

    m_pSocket = m_pServer->nextPendingConnection();
    if (!m_pSocket) return;

    connect(m_pSocket, &QBluetoothSocket::readyRead, this, &Translator::readyRead);
    connect(m_pSocket, &QBluetoothSocket::disconnected, this, &Translator::socketDisconnected);

    qDebug() << "Device Connected:" << m_pSocket->peerName();
    m_bSendStream = true;
}

void Translator::socketDisconnected()
{
    qDebug() << "Device Disconnected";
    m_bSendStream = false;
    m_pSocket->deleteLater();
    m_pSocket = nullptr;
}

void Translator::readyRead()
{
    if (!m_pSocket) return;
    QByteArray data = m_pSocket->readAll();
    // In the future, handle configuration commands here
    // qDebug() << "RX:" << data.toHex();
}

void Translator::timerEvent(QTimerEvent *pEvent)
{
    if (pEvent->timerId() == m_iRoutineTimer)
    {
        if (m_bSendStream && m_pSocket && m_pSocket->state() == QBluetoothSocket::ConnectedState) {
            sendDummyHeartbeat();
        }
    }
}

void Translator::sendDummyHeartbeat()
{
    // GDL90 Heartbeat Message (ID 0x00)
    // Byte 1: Status Byte 1. 0x81 = (Initialized | Battery Low/Ok) - setting top bit is important
    // Byte 2: Status Byte 2. 0x00
    // Byte 3-4: Timestamp (optional in some implementations, 0 is fine)
    // Byte 5-6: Message Counts (optional)
    
    QByteArray payload;
    payload.append((char)0x81); // Status: System Initialized
    payload.append((char)0x00); // Status 2
    payload.append((char)0x00); // Time LSB
    payload.append((char)0x00); // Time MSB
    payload.append((char)0x00); // Count
    payload.append((char)0x00); // Count

    QByteArray packet = buildGDL90(0x00, payload);
    m_pSocket->write(packet);
}

// ---------------------------------------------------------
// GDL90 PROTOCOL HELPERS
// ---------------------------------------------------------

// Standard CRC16-CCITT tableless implementation
quint16 Translator::crc16(const char *data, int len)
{
    quint16 crc = 0; // GDL90 starts with 0
    for(int i = 0; i < len; ++i) {
        crc ^= ((quint16)data[i] << 8);
        for(int j = 0; j < 8; ++j) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

QByteArray Translator::buildGDL90(quint8 msgId, const QByteArray &payload)
{
    QByteArray msg;
    msg.append((char)msgId);
    msg.append(payload);

    // Calculate CRC on the raw message (ID + Payload)
    quint16 crc = crc16(msg.constData(), msg.size());
    
    // Append CRC (Little Endian for GDL90)
    msg.append(crc & 0xFF);
    msg.append((crc >> 8) & 0xFF);

    // Perform Byte Stuffing (Escaping)
    QByteArray escapedMsg;
    escapedMsg.append((char)0x7E); // Start Flag

    for (int i = 0; i < msg.size(); ++i) {
        unsigned char c = (unsigned char)msg[i];
        if (c == 0x7E || c == 0x7D) {
            escapedMsg.append((char)0x7D);
            escapedMsg.append((char)(c ^ 0x20));
        } else {
            escapedMsg.append((char)c);
        }
    }

    escapedMsg.append((char)0x7E); // End Flag

    return escapedMsg;
}

// ---------------------------------------------------------
// STUBS
// ---------------------------------------------------------
void Translator::status(bool bStratux, bool bAHRS, bool bGPS) { Q_UNUSED(bStratux) Q_UNUSED(bAHRS) Q_UNUSED(bGPS) }
void Translator::situation(StratuxSituation s) { Q_UNUSED(s) }
void Translator::traffic(int iICAO, StratuxTraffic t) { Q_UNUSED(iICAO) Q_UNUSED(t) }