#ifndef PLOTTER1D_H
#define PLOTTER1D_H
#include <QDialog>
#include "qwt_plot_curve.h"
#include <vector>

namespace Ui {
class Plotter1D;
}

class Plotter1D : public QDialog
{
    Q_OBJECT
    
public:
    explicit Plotter1D(QWidget *parent = 0);
    ~Plotter1D();
    // add a new curve to the plotter
    void addCurve(const std::vector<double>& x, const std::vector<double>& y);

private:
    std::vector<double> _x;
    std::vector<double> _y;
    Ui::Plotter1D *ui;
};



#endif // PLOTTER1D_H
