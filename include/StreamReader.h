#ifndef STREAMREADER_H
#define STREAMREADER_H

#include <QObject>
#include <QWebSocket>

#include "StratuxStreams.h"

class StreamReader : QObject
{
    Q_OBJECT

public:
    StreamReader( QObject *parent = 0 );
    ~StreamReader();

    bool isConnected() const { return m_bConnected; }

    void connectStreams();
    void disconnectStreams();

signals:
    void newSituation( StratuxSituation );
    void newStatus( bool, bool, bool );
    void newTraffic( int, StratuxTraffic );

private slots:
    void situationUpdate( const QString &qsMessage );
    void statusUpdate( const QString &qsMessage );
    void trafficUpdate( const QString &qsMessage );
    void stratuxConnected();
    void stratuxDisconnected();

private:
    QWebSocket m_stratuxSituation;
    QWebSocket m_stratuxStatus;
    QWebSocket m_stratuxTraffic;
    bool m_bHaveMyPos;
    double m_dMyLat;
    double m_dMyLong;
    bool m_bAHRSStatus;
    bool m_bStratuxStatus;
    bool m_bGPSStatus;
    bool m_bConnected;

    void initSituation( StratuxSituation &situation );
    void initStatus( StratuxStatus &status );
    void initTraffic( StratuxTraffic &traffic );
};

#endif // STREAMREADER_H