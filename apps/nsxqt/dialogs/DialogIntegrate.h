// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXQT_DIALOGINTEGRATE_H
#define NSXQT_DIALOGINTEGRATE_H


#include <string>
#include <vector>

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

    void setIntegrators(const std::vector<std::string>& names);

    double peakScale() const;
    double bkgBegin() const;
    double bkgEnd() const;
    double dMin() const;
    double dMax() const;
    double radius() const;
    double nframes() const;

    bool fitCenter() const;
    bool fitCov() const;

    std::string integrator() const;

private:
    Ui::DialogIntegrate* _ui;   
};
#endif // NSXQT_DIALOGINTEGRATE_H
