#include "include/NSXQtApp.h"

#include <QtDebug>

#include "Logger.h"

NSXQtApp::NSXQtApp(int &argc, char *argv[]) : QApplication(argc,argv)
{
}

bool NSXQtApp::notify(QObject* receiver, QEvent* event)
{
    try
    {
        return QApplication::notify(receiver,event);
    }
    catch (std::exception& e)
    {
        qWarning()<<e.what();
        return false;
    }
}
