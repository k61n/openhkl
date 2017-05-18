// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXQT_DIALOGINTEGRATE_H_
#define NSXQT_DIALOGINTEGRATE_H_

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

    double peakScale() const { return _peakScale; }
    double backgroundScale() const { return _bkgScale; }
    bool updateShape() const { return true; }

private:
    double _peakScale;
    double _bkgScale;
    Ui::DialogIntegrate* _ui;

};
#endif // NSXQT_DIALOGINTEGRATE_H_
