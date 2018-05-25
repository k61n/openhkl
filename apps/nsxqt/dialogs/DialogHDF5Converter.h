#pragma once

#include <QDialog>

#include <nsxlib/InstrumentTypes.h>

namespace Ui {
class DialogHDF5Converter;
}

class DialogHDF5Converter : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHDF5Converter(nsx::sptrExperiment experiment, QWidget* parent = 0);
    ~DialogHDF5Converter();

private slots:

    void browseInputNumors();
    void browseOutputDirectory();
    void convert();

private:
    Ui::DialogHDF5Converter *ui;

    nsx::sptrExperiment _experiment;
};
