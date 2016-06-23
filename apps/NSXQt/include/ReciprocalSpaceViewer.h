#ifndef RECIPROCALSPACEVIEWER_H
#define RECIPROCALSPACEVIEWER_H

#include <vector>

#include <QDialog>

namespace Ui
{
class ReciprocalSpaceViewer;
}

namespace SX
{
namespace Instrument
{
class Experiment;
}
namespace Data
{
class IData;
}
}

class ReciprocalSpaceViewer : public QDialog
{
    Q_OBJECT

public:
    explicit ReciprocalSpaceViewer(SX::Instrument::Experiment* experiment, QWidget *parent = 0);

    void setData(const std::vector<SX::Data::IData*>& data);

    ~ReciprocalSpaceViewer();

private slots:
    void on_view_clicked();

private:
    Ui::ReciprocalSpaceViewer *ui;

    SX::Instrument::Experiment* _experiment;

    std::vector<SX::Data::IData*> _data;
};

#endif // RECIPROCALSPACEVIEWER_H
