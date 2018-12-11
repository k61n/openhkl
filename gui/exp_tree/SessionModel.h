#pragma once

#include <QStandardItemModel>

#include <core/DataSet.h>
#include <core/Experiment.h>
#include <core/Peak3D.h>

class DataSet;
class ExperimentItem;

class SessionModel: public QStandardItemModel {
    Q_OBJECT

public:
    SessionModel();

    ~SessionModel();

    nsx::PeakList peaks(nsx::sptrDataSet data) const;

    void selectData(nsx::sptrDataSet data);

    ExperimentItem* selectExperiment(nsx::sptrDataSet data);

    void addExperiment(nsx::sptrExperiment experiment);

signals:
    void plotData(nsx::sptrDataSet);

    void inspectWidget(QWidget*);

    void updatePeaks();

    void signalSelectedDataChanged(nsx::sptrDataSet, int frame);

    void signalSelectedPeakChanged(nsx::sptrPeak3D peak);

    void signalEnabledPeakChanged(nsx::sptrPeak3D peak);

    void signalMaskedPeaksChanged(const nsx::PeakList& peaks);

    void signalUnitCellRemoved(nsx::sptrUnitCell unit_cell);

public slots:
    void onItemChanged(QStandardItem* item);
};
