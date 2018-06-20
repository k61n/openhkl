#pragma once

#include <QDialog>

#include <nsxlib/DataTypes.h>

namespace Ui {
class DialogHDF5Converter;
}

class DialogHDF5Converter : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHDF5Converter(const nsx::DataList& numors, QWidget* parent = 0);
    ~DialogHDF5Converter();

private slots:

    void browseOutputDirectory();
    void convert();

private:
    Ui::DialogHDF5Converter *ui;

    nsx::DataList _numors;
};
