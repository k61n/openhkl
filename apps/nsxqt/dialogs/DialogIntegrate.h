// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXQT_DIALOGINTEGRATE_H
#define NSXQT_DIALOGINTEGRATE_H

#include <QDialog>

namespace Ui {
class DialogIntegrate;
}

class DialogIntegrate : public QDialog
{
    Q_OBJECT

public:
    DialogIntegrate(QWidget *parent = 0);
    ~DialogIntegrate();

    double peakScale() const;
    double backgroundScale() const;
    double dMin() const;
    double dMax() const;
    double radius() const;
    double nframes() const;

private:
    Ui::DialogIntegrate* _ui;

};
#endif // NSXQT_DIALOGINTEGRATE_H
