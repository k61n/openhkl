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
    auto logfile = std::ofstream("nsxqt_log.txt");
    nsx::wrapper()->addWrapper(new nsx::StdStreamWrapper(logfile));

    nsx::setArgc(argc);
    nsx::setArgv(argv);

    NSXQtApp a(argc, argv);

    // Ensure . is used rather than , for float and double boxes
    QLocale::setDefault(QLocale::c());
    MainWindow w;

    w.showMaximized();
    w.show();

    return a.exec();
}
