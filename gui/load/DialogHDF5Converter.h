#pragma once

#include <QDialog>

#include <core/DataTypes.h>

class QAbstractButton;

namespace Ui {
class DialogHDF5Converter;
}

class DialogHDF5Converter : public QDialog {
    Q_OBJECT

public:
    explicit DialogHDF5Converter(const nsx::DataList& numors, QWidget* parent = 0);
    ~DialogHDF5Converter();

private slots:
    void slotActionClicked(QAbstractButton* button);

    void browseOutputDirectory();

private:
    void convert();

private:
    Ui::DialogHDF5Converter* _ui;

    nsx::DataList _numors;
};
