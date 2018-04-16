#include <fstream>

#include <QSplashScreen>
#include <QDesktopWidget>
#include <QPainter>
#include <QTimer>
#include <QSplashScreen>

#include <nsxlib/AggregateStreamWrapper.h>
#include <nsxlib/Logger.h>
#include <nsxlib/StdStreamWrapper.h>

#include "MainWindow.h"
#include "NSXQtApp.h"

int main(int argc, char* argv[])
{
    NSXQtApp a(argc, argv);

    // Ensure . is used rather than , for float and double boxes
    QLocale::setDefault(QLocale::c());
    MainWindow w;

    nsx::info()<<"NSXQt session started";

    w.showMaximized();
    w.show();

    return a.exec();
}
