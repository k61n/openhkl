#include <QSplashScreen>
#include <QDesktopWidget>
#include <QPainter>
#include <QTimer>
#include <QSplashScreen>

#include "MainWindow.h"
#include "NSXQtApp.h"

int main(int argc, char *argv[])
{
    NSXQtApp a(argc, argv);

    // Ensure . is used rather than , for float and double boxes
    QLocale::setDefault(QLocale::c());

    // Show splash
    QImage splashScrImage(":/resources/splashScreen.png");
    QPixmap Logo;
    Logo.convertFromImage(splashScrImage);
    MainWindow w;
    QSplashScreen splashScrWindow (&w, Logo, Qt::WindowStaysOnTopHint);
    QSize screenSize = QApplication::desktop()->geometry().size();
    splashScrWindow.move(screenSize.width()/2-300,screenSize.height()/2-150);
    splashScrWindow.show();
    QTimer::singleShot(2000, &splashScrWindow ,SLOT(close()));
    w.showMaximized();
    w.show();

    return a.exec();
}
