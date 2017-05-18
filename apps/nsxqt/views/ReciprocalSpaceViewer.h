#ifndef NSXQT_RECIPROCALSPACEVIEWER_H
#define NSXQT_RECIPROCALSPACEVIEWER_H

#include <memory>
#include <vector>

#include <QDialog>

namespace Ui
{
class ReciprocalSpaceViewer;
}

namespace nsx
{
class DataSet;
class Experiment;
}

class ReciprocalSpaceViewer : public QDialog
{
    Q_OBJECT

public:
    explicit ReciprocalSpaceViewer(std::shared_ptr<nsx::Experiment> experiment, QWidget *parent = 0);

    void setData(const std::vector<std::shared_ptr<nsx::DataSet>>& data);

    ~ReciprocalSpaceViewer();

private slots:
    void on_view_clicked();

private:
    Ui::ReciprocalSpaceViewer *ui;

    std::shared_ptr<nsx::Experiment> _experiment;
    std::vector<std::shared_ptr<nsx::DataSet>> _data;
};

#endif // NSXQT_RECIPROCALSPACEVIEWER_H
