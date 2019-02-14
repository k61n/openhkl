#pragma once

#include <QApplication>

class NSXQtApp : public QApplication {
public:
  NSXQtApp(int &argc, char *argv[]);

private:
  bool notify(QObject *receiver, QEvent *event);
};
