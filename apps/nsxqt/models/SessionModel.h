#pragma once

#include <memory>

#include <QStandardItemModel>

#include <nsxlib/DataSet.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Peak3D.h>

class DataSet;
class Experiment;
class ExperimentItem;
class Peak3D;

class SessionModel: public QStandardItemModel {

    Q_OBJECT

public:

    explicit SessionModel();

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
