#ifndef PLOTTER1D_H
#define PLOTTER1D_H

#include <QDialog>

namespace Ui {
class Plotter1D;
}

class Plotter1D : public QDialog
{
    Q_OBJECT
    
public:
    explicit Plotter1D(QWidget *parent = 0);
    ~Plotter1D();
    
private:
    Ui::Plotter1D *ui;
};

#endif // PLOTTER1D_H
