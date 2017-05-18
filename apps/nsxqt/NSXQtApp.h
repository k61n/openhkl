#ifndef NSXQT_NSXQTAPP_H
#define NSXQT_NSXQTAPP_H

#include <QApplication>

class NSXQtApp : public QApplication
{
public:
    NSXQtApp(int &argc, char *argv[]);

private:
    bool notify(QObject* receiver, QEvent* event);
};

#endif // NSXQT_NSXQTAPP_H
