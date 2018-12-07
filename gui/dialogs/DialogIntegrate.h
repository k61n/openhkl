#pragma once

#include <string>
#include <vector>

#include <QDialog>

#include <nsxlib/CrystalTypes.h>

namespace Ui {
class DialogIntegrate;
}

class DialogIntegrate : public QDialog
{
    Q_OBJECT

public:
    DialogIntegrate(const nsx::PeakList& peaks, QWidget *parent = 0);
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
