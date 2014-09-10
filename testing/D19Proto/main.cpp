#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QPainter>
#include <QTimer>
#include <QSplashScreen>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QImage splashScrImage(":/resources/splashScreen.png");
    QPixmap Logo;
    Logo.convertFromImage(splashScrImage);
    MainWindow w;
    QSplashScreen splashScrWindow (&w, Logo, Qt::WindowStaysOnTopHint);
    QSize screenSize = QApplication::desktop()->geometry().size();
    splashScrWindow.move(screenSize.width()/2-300,screenSize.height()/2-150);
    splashScrWindow.show();
    QTimer::singleShot(3500, &splashScrWindow ,SLOT(close()));
    w.showMaximized();
    w.show();

    return a.exec();
}
