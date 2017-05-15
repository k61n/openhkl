#ifndef RECIPROCALSPACEVIEWER_H
#define RECIPROCALSPACEVIEWER_H

#include <vector>
#include <memory>

#include <QDialog>

namespace Ui
{
class ReciprocalSpaceViewer;
}

namespace nsx
{
namespace Instrument
{
class Experiment;
}
namespace Data
{
class DataSet;
}
}

class ReciprocalSpaceViewer : public QDialog
{
    Q_OBJECT

public:
    explicit ReciprocalSpaceViewer(std::shared_ptr<nsx::Instrument::Experiment> experiment, QWidget *parent = 0);

    void setData(const std::vector<std::shared_ptr<nsx::Data::DataSet>>& data);

    ~ReciprocalSpaceViewer();

private slots:
    void on_view_clicked();

private:
    Ui::ReciprocalSpaceViewer *ui;

    std::shared_ptr<nsx::Instrument::Experiment> _experiment;
    std::vector<std::shared_ptr<nsx::Data::DataSet>> _data;
};

#endif // RECIPROCALSPACEVIEWER_H
