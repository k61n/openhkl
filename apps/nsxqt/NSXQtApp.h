#ifndef NSXQTAPP_H
#define NSXQTAPP_H

#include <QApplication>

class NSXQtApp : public QApplication
{
public:
    NSXQtApp(int &argc, char *argv[]);

private:
    bool notify(QObject* receiver, QEvent* event);
};

#endif // NSXQTAPP_H
