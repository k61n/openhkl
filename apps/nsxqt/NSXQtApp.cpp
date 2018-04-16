#include <nsxlib/Logger.h>
#include <nsxlib/Path.h>

#include <QApplication>

#include "NSXQtApp.h"

NSXQtApp::NSXQtApp(int &argc, char *argv[]) : QApplication(argc,argv)
{
    auto executable_dir_path = QCoreApplication::applicationDirPath().toStdString();

    nsx::setArgv(nsx::fileDirname(executable_dir_path).c_str());

    this->setApplicationDisplayName("nsxqt");
    this->setApplicationName("nsxqt");
}

bool NSXQtApp::notify(QObject* receiver, QEvent* event)
{
    try {
        return QApplication::notify(receiver, event);
    } catch (std::exception& e) {
        nsx::error() << e.what();
        return false;
    }
}
