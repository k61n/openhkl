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
    SX::Data::MMILLAsciiReader mm(filename.c_str());
    mm.readMetaDataBlock();
    _frames.resize(mm.nBlocks());
    int executed=0;
    #pragma omp parallel for
    for (std::size_t i=0;i<mm.nBlocks();++i)
    {


        _frames[i]=std::move(mm.readBlock(i));

    #pragma omp critical
    bar->setValue(executed++*100/mm.nBlocks());
    }

    }
std::vector<vint> _frames;

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
