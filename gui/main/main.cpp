
#include "gui/mainwindow.h"
#include <QApplication>


int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    app.setApplicationName("nsxgui");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("nsx");

    MainWindow* mainWin = new MainWindow{};

    return app.exec();
}
