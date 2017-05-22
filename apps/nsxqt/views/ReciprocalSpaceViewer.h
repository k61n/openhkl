#ifndef NSXQT_RECIPROCALSPACEVIEWER_H
#define NSXQT_RECIPROCALSPACEVIEWER_H

#include <vector>

#include <QDialog>

#include <nsxlib/data/DataTypes.h>
#include <nsxlib/instrument/InstrumentTypes.h>

namespace Ui
{
class ReciprocalSpaceViewer;
}

class ReciprocalSpaceViewer : public QDialog
{
    Q_OBJECT

public:
    explicit ReciprocalSpaceViewer(nsx::sptrExperiment experiment, QWidget *parent = 0);

    void setData(const nsx::DataList& data);

    ~ReciprocalSpaceViewer();

private slots:
    void on_view_clicked();

private:
    Ui::ReciprocalSpaceViewer *ui;

    nsx::sptrExperiment _experiment;
    nsx::DataList _data;
};

#endif // NSXQT_RECIPROCALSPACEVIEWER_H
