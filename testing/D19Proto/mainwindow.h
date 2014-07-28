#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <boost/numeric/ublas/matrix.hpp>
#include <QMainWindow>
#include <QGraphicsScene>
#include <vector>
#include "ILLAsciiDataReader.h"
#include <QProgressBar>
#include <unordered_map>
#include "Logger.h"
#include <QMenu>
#include <DialogPeakFind.h>
#include "Data.h"

class QListWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_open_triggered();

    void on_action_peak_find_triggered();

    void on_horizontalScrollBar_valueChanged(int value);

    void on_dial_valueChanged(int value);

    void on_spinBox_max_valueChanged(int arg1);

    void deleteNumors();

    void on_numor_Widget_itemDoubleClicked(QListWidgetItem *item);

    void on_numor_Widget_itemActivated(QListWidgetItem *item);

    void ShowContextMenu(const QPoint& pos);

    void on_textLogger_customContextMenuRequested(const QPoint &pos);


private:
    void updatePlot();
    Ui::MainWindow *ui;
    QGraphicsScene* scene;
    std::unordered_map<std::string,Data> _data;
    QMenu* loggerContextMenu;
protected:
};

#endif // MAINWINDOW_H
