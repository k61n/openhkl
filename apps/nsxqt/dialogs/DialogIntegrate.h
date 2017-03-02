// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_DIALOGINTEGRATE_H_
#define NSXTOOL_DIALOGINTEGRATE_H_

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

    double getPeakScale() const { return _peakScale; }
    double getBackgroundScale() const { return _bkgScale; }

private:
    double _peakScale;
    double _bkgScale;
    Ui::DialogIntegrate* _ui;

};
#endif // NSXTOOL_DIALOGINTEGRATE_H_
