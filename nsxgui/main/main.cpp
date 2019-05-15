
#include "nsxgui/gui/mainwin.h"
#include <QApplication>
#include "nsxgui/qcr/engine/console.h"
#include "nsxgui/qcr/engine/logger.h"
#include "nsxgui/gui/models/session.h"


int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    app.setApplicationName("nsxgui");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("nsx");

    Logger logger{"nsxgui.log"};
    Console console;
    Session session;

    MainWin* mainWin = new MainWin{};

    return app.exec();
}
