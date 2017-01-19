#ifndef PEAKTABLEVIEW_H
#define PEAKTABLEVIEW_H

#include <map>
#include <string>
#include <tuple>
#include <memory>

#include <QFileDialog>
#include <QMenu>
#include <QTableView>
#include <IData.h>
#include <MainWindow.h>

class PeakCustomPlot;
class QContextMenuEvent;
class QMouseEvent;

namespace SX
{
namespace Crystal
{
class Peak3D;
using sptrPeak3D=std::shared_ptr<Peak3D>;
}
}

using SX::Crystal::sptrPeak3D;

class PeakTableView : public QTableView
{
    Q_OBJECT
public:

    explicit PeakTableView(QWidget* parent = 0);
    void contextMenuEvent(QContextMenuEvent *);

    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event) const;

signals:
    void plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&);
    void plotPeak(sptrPeak3D);

public slots:
    //! Write the current list to FullProf
    void writeFullProf();
    //! Write the currrent list to ShelX
    void writeShelX();
    //! Normalize to monitor.
    void normalizeToMonitor();
    //! Plot as function of parameter. Needs to be a numeric type
    void plotAs(const std::string& key);
    //! Search peaks with hkl matching part of the string. Text must represent h,k,l values separated by white spaces
    void showPeaksMatchingText(QString text);
    //! Plot selected peak
    void plotSelectedPeak(int index);

    void clearSelectedPeaks();
    void selectAllPeaks();
    void selectValidPeaks();
    void selectUnindexedPeaks();
    void togglePeaksSelection();

private:
    bool checkBeforeWritting();
    std::string getPeaksRange() const;

private:

    //! Flag indicating that data have been normalized either to time or monitor.
    bool _normalized;

};

#endif // PEAKTABLEVIEW_H
