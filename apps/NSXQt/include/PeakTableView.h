#ifndef PEAKTABLEVIEW_H
#define PEAKTABLEVIEW_H

#include <map>
#include <string>
#include <tuple>

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
}
}

class PeakTableView : public QTableView
{
    Q_OBJECT
public:
    explicit PeakTableView(QWidget* parent = 0);
    void setData(std::vector<SX::Data::IData*>);
    void contextMenuEvent(QContextMenuEvent *);
    void mousePressEvent(QMouseEvent *event);
signals:
    void plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&);
    void plotPeak(SX::Crystal::Peak3D*);
public slots:
    //! Slection of peak is changed
    void peakChanged(QModelIndex current,QModelIndex last);
    //! Display context menu
//    void customMenuRequested(QPoint pos);
    //! Sort the current table by column inde
    void sortByColumn(int column);
    //! Sort equivalent reflections
    void sortEquivalents();
    //! Write the current list to FullProf
    void writeFullProf();
    //! Write the currrent list to ShelX
    void writeShelX();
    //! Normalize to monitor.
    void normalizeToMonitor();
    //! Change selected status of peak when double-clicked
    void deselectPeak(QModelIndex index);
    //! Plot as function of parameter. Needs to be a numeric type
    void plotAs(const std::string& key);
private:
    void sortByHKL(bool up);
    void sortByRawIntensity(bool up);
    void sortByCorrectedIntensity(bool up);
    void sortByNumor(bool up);
    void sortBySelected(bool up);
    void sortByLorentzFactor(bool up);
    void sortByTransmission(bool up);
    bool checkBeforeWritting();
    void constructTable();
    std::string getPeaksRange() const;
    std::vector<std::reference_wrapper<SX::Crystal::Peak3D>> _peaks;
    //! Which column is sorted and up or down
    std::tuple<int,bool> _columnUp;
    PeakCustomPlot* _plotter;
    //! Flag indicating that data have been normalized either to time or monitor.
    bool _normalized;
};

#endif // PEAKTABLEVIEW_H
