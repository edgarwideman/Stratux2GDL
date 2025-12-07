#ifndef STREAMREADER_H
#define STREAMREADER_H

#include <QObject>
#include "StubTypes.h"

class StreamReader : public QObject
{
    Q_OBJECT
public:
    explicit StreamReader(QObject *parent = nullptr);
    void connectStreams() {} // Empty for now
    void disconnectStreams() {} // Empty for now
    bool isConnected() { return false; } // Always false for this test

signals:
    void newSituation(StratuxSituation s);
    void newStatus(bool bStratux, bool bAHRS, bool bGPS);
    void newTraffic(int iICAO, StratuxTraffic t);
};

#endif // STREAMREADER_H