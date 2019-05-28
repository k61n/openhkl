
#include "nsxgui/gui/mainwin.h"
#include <QApplication>
#include <QCR/engine/console.h>
#include <QCR/engine/logger.h>
#include "nsxgui/gui/models/session.h"


int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    app.setApplicationName("nsxgui");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("nsx");

    Logger logger{"nsxgui.log"};
    Console console;
    Session session;

    new MainWin{};

    return app.exec();
}
