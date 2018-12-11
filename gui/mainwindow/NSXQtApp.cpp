#include <core/Logger.h>
#include <core/Version.h>

#include <QApplication>

#include "NSXQtApp.h"

NSXQtApp::NSXQtApp(int& argc, char* argv[]) : QApplication(argc, argv)
{
    //    this->setApplicationDisplayName(QString::fromStdString("NSXQt "+nsx::__version__));
    this->setApplicationName("nsxqt");
}

bool NSXQtApp::notify(QObject* receiver, QEvent* event)
{
    try {
        return QApplication::notify(receiver, event);
    } catch (std::exception& e) {
        nsx::error() << e.what();
        return false;
    }
}
