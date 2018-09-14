#include <fstream>
#include <locale>

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
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QGuiApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    #endif
    NSXQtApp a(argc, argv);

    // Ensure . is used rather than , for float and double boxes
    QLocale::setDefault(QLocale::c());

    std::setlocale(LC_ALL, "C");

    MainWindow* main_window = MainWindow::create(nullptr);

    nsx::info()<<"NSXQt session started";

    main_window->showMaximized();
    main_window->show();

    return a.exec();
}
