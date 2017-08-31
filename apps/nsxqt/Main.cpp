#include <fstream>

#include <QSplashScreen>
#include <QDesktopWidget>
#include <QPainter>
#include <QTimer>
#include <QSplashScreen>

#include <nsxlib/logger/AggregateStreamWrapper.h>
#include <nsxlib/logger/Logger.h>
#include <nsxlib/logger/StdStreamWrapper.h>
#include <nsxlib/utils/Path.h>

#include "MainWindow.h"
#include "NSXQtApp.h"

int main(int argc, char* argv[])
{
    nsx::setArgc(argc);
    nsx::setArgv(argv);

    NSXQtApp a(argc, argv);

    // Ensure . is used rather than , for float and double boxes
    QLocale::setDefault(QLocale::c());
    MainWindow w;

    nsx::info()<<"NSXQt session started";

    w.showMaximized();
    w.show();

    return a.exec();
}
