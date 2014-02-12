#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <boost/numeric/ublas/matrix.hpp>
#include <QMainWindow>
#include <QGraphicsScene>
#include <vector>
#include "MMILLAsciiReader.h"
#include <QProgressBar>

using namespace boost::numeric::ublas;

typedef std::vector<int> vint;

struct Data
{
    Data()
    {}
    void fromFile(const std::string& filename,QProgressBar* bar)
    {
        mm.mapFile(filename.c_str());
        _nblocks=mm.nBlocks();
        _frames=std::move(mm.readBlock(0));

    }
    void readBlock(int i)
    {
        _frames=std::move(mm.readBlock(i));
    }
    int _nblocks;
    SX::Data::MMILLAsciiReader mm;
    vint _frames;

};


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
    void on_action_Open_triggered();
    void on_horizontalScrollBar_valueChanged(int value);

    void on_dial_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QGraphicsScene* scene;
    Data _data;
};

#endif // MAINWINDOW_H
