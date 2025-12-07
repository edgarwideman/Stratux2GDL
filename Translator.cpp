/*
Stratux AHRS Display
(c) 2018 Allen K. Lair, Unexploded Minds
Modified for GDL39 emulation 2025
*/

#include <QTimer>
#include <QDateTime>
#include <QBluetoothUuid>

#include "Translator.h"
#include "StreamReader.h"

// Setup minimal UI elements and make the connections
Translator::Translator()
    : QObject( 0 ),
      m_pStreamReader( new StreamReader( this ) ),
      m_pServer( new QBluetoothServer(QBluetoothUuid::SerialPort, this) ),
      m_pSocket( nullptr ),
      m_bAHRS( false ),
      m_bGPS( false ),
      m_bStratux( false ),
      m_bSendStream( false )
{
    m_lastStatusUpdate = QDateTime::currentDateTime();

    connect( m_pStreamReader, SIGNAL( newSituation( StratuxSituation ) ), this, SLOT( situation( StratuxSituation ) ) );
    connect( m_pStreamReader, SIGNAL( newStatus( bool, bool, bool ) ), this, SLOT( status( bool, bool, bool ) ) );
    connect( m_pStreamReader, SIGNAL( newTraffic( int, StratuxTraffic ) ), this, SLOT( traffic( int, StratuxTraffic ) ) );
    m_pStreamReader->connectStreams();

    connect( m_pServer, &QBluetoothServer::newConnection, this, &Translator::newConnection );
    m_pServer->listen(QBluetoothUuid::SerialPort, "GDL39");

    qDebug() << "Bluetooth SPP listening as GDL39";

    m_iRoutineTimer = startTimer( 1000 );
}

// Delete the stream reader
Translator::~Translator()
{
    delete m_pStreamReader;
    m_pStreamReader = 0;
}

// Status stream is received here
void Translator::status( bool bStratux, bool bAHRS, bool bGPS )
{
    m_bStratux = bStratux;
    m_bAHRS = bAHRS;
    m_bGPS = bGPS;
    m_lastStatusUpdate = QDateTime::currentDateTime();
}

void Translator::timerEvent( QTimerEvent *pEvent )
{
    if( pEvent == 0 )
        return;

    if( pEvent->timerId() == m_iRoutineTimer )
    {
        // Reconnect Stratux websockets if needed
        if( (!m_pStreamReader->isConnected()) || (m_lastStatusUpdate.secsTo( QDateTime::currentDateTime() ) > 10) )
        {
            m_pStreamReader->disconnectStreams();
            m_pStreamReader->connectStreams();
            qDebug() << "Stratux local websockets reconnected";
        }

        // Send dummy heartbeat if connected and send stream enabled
        if( m_pSocket && m_pSocket->state() == QBluetoothSocket::ConnectedState && m_bSendStream )
            sendDummyHeartbeat();
    }
}

// Situation update
void Translator::situation( StratuxSituation s )
{
    if( !m_bSendStream || !m_pSocket || m_pSocket->state() != QBluetoothSocket::ConnectedState )
        return;

    // TODO: Build and send GDL90 ownship and attitude messages from s data
    qDebug() << "Situation update, to be implemented";
}

// Traffic update
void Translator::traffic( int iICAO, StratuxTraffic t )
{
    if( !m_bSendStream || !m_pSocket || m_pSocket->state() != QBluetoothSocket::ConnectedState )
        return;

    // TODO: Build and send GDL90 traffic message from t data
    qDebug() << "Traffic update, to be implemented";
}

// New Bluetooth connection
void Translator::newConnection()
{
    m_pSocket = m_pServer->nextPendingConnection();
    if (m_pSocket)
    {
        connect(m_pSocket, &QBluetoothSocket::readyRead, this, &Translator::readyRead);
        connect(m_pSocket, &QBluetoothSocket::disconnected, this, &Translator::disconnected);
        m_bSendStream = true; // Enable stream; add handshake here if needed
        qDebug() << "Bluetooth device connected";
    }
}

// Ready read from socket (for handshake or commands)
void Translator::readyRead()
{
    // TODO: Handle handshake response
    QByteArray data = m_pSocket->readAll();
    qDebug() << "Received data:" << data;
}

// Disconnected
void Translator::disconnected()
{
    m_bSendStream = false;
    m_pSocket->deleteLater();
    m_pSocket = nullptr;
    qDebug() << "Bluetooth device disconnected";
}

// Build GDL90 message
QByteArray Translator::buildGDL90(byte id, QByteArray data)
{
    QByteArray msg;
    msg.append(0x7E);
    msg.append(id);
    msg += data;

    unsigned short crc = crc16(msg.constData() + 1, msg.size() - 1);
    msg.append((crc >> 8) & 0xFF);
    msg.append(crc & 0xFF);
    msg.append(0x7E);

    // Escape 0x7E and 0x7D
    QByteArray escaped;
    escaped.append(0x7E);
    for (int i = 1; i < msg.size() - 1; ++i) {
        if (msg[i] == 0x7E || msg[i] == 0x7D) {
            escaped.append(0x7D);
            escaped.append(msg[i] ^ 0x20);
        } else {
            escaped.append(msg[i]);
        }
    }
    escaped.append(0x7E);

    return escaped;
}

// CRC16 for GDL90
unsigned short Translator::crc16(const char* data, int length)
{
    unsigned short crc = 0xFFFF;
    for (int i = 0; i < length; ++i) {
        crc ^= (unsigned short) data[i] << 8;
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc ^ 0xFFFF;
}

// Send dummy heartbeat
void Translator::sendDummyHeartbeat()
{
    QByteArray data(7, 0);
    data[0] = 0x81; // Status byte 1 example
    data[1] = 0x00; // Status byte 2
    data[2] = 0x00; // Timestamp LSB
    data[3] = 0x00; // Timestamp MSB
    data[4] = 0x00; // Uplink count
    data[5] = 0x00; // Basic/long count
    data[6] = 0x00; // CS count

    QByteArray msg = buildGDL90(0x00, data);
    m_pSocket->write(msg);
    m_pSocket->waitForBytesWritten(250);
}