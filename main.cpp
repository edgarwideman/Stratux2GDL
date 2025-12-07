#include <QCoreApplication>

#include "Translator.h"

int main( int argc, char *argv[] )
{
    QCoreApplication coreApp( argc, argv );
    Translator       trans;

    Q_UNUSED( trans )

    QCoreApplication::setOrganizationName( "Unexploded Minds" );
    QCoreApplication::setOrganizationDomain( "unexplodedminds.com" );
    QCoreApplication::setApplicationName( "Stratux2GDL39" );

    qDebug() << "\nStratux2GDL39 (c) 2018 Unexploded Minds, modified 2025\n";

    // Set Bluetooth device name and make discoverable (run with sudo if needed)
    system("hciconfig hci0 name 'GDL 39'");
    system("hciconfig hci0 piscan");

    return coreApp.exec();
}