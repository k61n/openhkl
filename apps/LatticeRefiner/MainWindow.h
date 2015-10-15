#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

#include <QStandardItemModel>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slot_changed(const QModelIndex& topLeft, const QModelIndex& bottomRight);

private:
    Ui::MainWindow *ui;

    std::vector<QStandardItemModel*> _models;
};

#endif // MAINWINDOW_H
