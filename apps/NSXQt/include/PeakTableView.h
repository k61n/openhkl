#ifndef PEAKTABLEVIEW_H
#define PEAKTABLEVIEW_H
#include <QTableView>
#include <IData.h>
#include <QMenu>
#include <QFileDialog>
#include <MainWindow.h>
#include <tuple>

class PeakTableView : public QTableView
{
    Q_OBJECT
public:
    explicit PeakTableView(MainWindow* main,QWidget* parent = 0);
    void setData(const std::vector<SX::Data::IData*>);
signals:
    void plot2DUpdate(int numor,int frame);
public slots:
    //! Slection of peak is changed
    void peakChanged(QModelIndex current,QModelIndex last);
    //! Plot the peak at index i in vector
    void plotPeak(int i);
    //! Display context menu
    void customMenuRequested(QPoint pos);
    //! Sort the current table by column inde
    void sortByColumn(int column);
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
    void sortByIntensity(bool up);
    void sortByNumor(bool up);
    void sortBySelected(bool up);
    MainWindow* _main;
    void constructTable();
    std::vector<std::reference_wrapper<SX::Crystal::Peak3D>> _peaks;
    //! Which column is sorted and up or down
    std::tuple<int,bool> _columnUp;
    QIcon _sortUpIcon;
    QIcon _sortDownIcon;
};

#endif // PEAKTABLEVIEW_H
