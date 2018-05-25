#pragma once

#include <QDialog>

#include <nsxlib/InstrumentTypes.h>

namespace Ui {
class NumorsConversionDialog;
}

class NumorsConversionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NumorsConversionDialog(nsx::sptrExperiment experiment, QWidget* parent = 0);
    ~NumorsConversionDialog();

private slots:

    void browseInputNumors();
    void browseOutputDirectory();
    void convert();

private:
    Ui::NumorsConversionDialog *ui;

    nsx::sptrExperiment _experiment;
};
