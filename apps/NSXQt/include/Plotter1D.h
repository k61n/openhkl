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
    void addCurve(const QVector<double>& x, const QVector<double>& y, const QVector<double>& e, QColor color=QColor("blue"));
    void modifyCurve(int number, const QVector<double>& x, const QVector<double>& y, const QVector<double>& e);
    void removeCurve(int idx);
    int nGraphs() const;
    void clear();
    
private:
    Ui::Plotter1D *ui;
};

#endif // PLOTTER1D_H
