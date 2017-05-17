#include <QSplashScreen>
#include <QDesktopWidget>
#include <QPainter>
#include <QTimer>
#include <QSplashScreen>

#include <nsxlib/utils/Path.h>

#include "MainWindow.h"
#include "NSXQtApp.h"

int main(int argc, char *argv[])
{
    nsx::Path::setArgv(argc, argv);
    NSXQtApp a(argc, argv);

    // Ensure . is used rather than , for float and double boxes
    QLocale::setDefault(QLocale::c());
    MainWindow w;

    w.showMaximized();
    w.show();

    return a.exec();
}
