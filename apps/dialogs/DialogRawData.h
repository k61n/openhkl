#pragma once

#include <QDialog>

namespace Ui {
class DialogRawData;
}

class DialogRawData : public QDialog {
    Q_OBJECT

public:
    explicit DialogRawData(QWidget* parent = 0);
    ~DialogRawData();

    double wavelength();
    double deltaChi();
    double deltaOmega();
    double deltaPhi();

    bool swapEndian();
    bool rowMajor();
    int bpp();

    void setWavelength(double new_wavelength);

private:
    Ui::DialogRawData* ui;
};
