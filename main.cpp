#include <QCoreApplication>
#include <QDebug>
#include "Translator.h"

int main(int argc, char *argv[])
{
    QCoreApplication coreApp(argc, argv);
    
    QCoreApplication::setOrganizationName("Edgar Wideman");
    QCoreApplication::setApplicationName("Stratux2GDL39");

    qDebug() << "------------------------------------------";
    qDebug() << " Stratux2GDL39 Service Starting";
    qDebug() << " Note: Ensure you run with 'sudo' to set BT name";
    qDebug() << "------------------------------------------";

    // Attempt to set name to "GDL 39" so Garmin pilot recognizes it
    // The spaces are important for some versions of the app.
    // Try "GDL 39" or "GDL39"
    int res = system("hciconfig hci0 name 'GDL 39'");
    if(res != 0) qDebug() << "Warning: Failed to set BT name. Manual setup may be required.";
    
    system("hciconfig hci0 piscan"); // Make discoverable

    Translator trans;

    return coreApp.exec();
}