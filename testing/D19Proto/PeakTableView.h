#ifndef PEAKTABLEVIEW_H
#define PEAKTABLEVIEW_H
#include <QTableView>
#include <Data.h>
#include <QMenu>
#include <PeakPlotter.h>
#include <QFileDialog>
class PeakTableView : public QTableView
{
    Q_OBJECT
public:
    explicit PeakTableView(QWidget* parent = 0);
    void setData(const std::vector<Data*>);
signals:

public slots:
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
private:
    PeakPlotter* _plotter;
    void constructTable();
    std::vector<std::reference_wrapper<const SX::Geometry::Peak3D>> _peaks;
};

#endif // PEAKTABLEVIEW_H
