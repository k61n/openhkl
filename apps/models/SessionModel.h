#pragma once

#include <QStandardItemModel>

#include <build/core/include/core/CrystalTypes.h>
#include <build/core/include/core/DataTypes.h>

class ExperimentItem;

class SessionModel : public QStandardItemModel {

  Q_OBJECT

public:
  explicit SessionModel();

  ~SessionModel();

  nsx::PeakList peaks(nsx::sptrDataSet data) const;

  void selectData(nsx::sptrDataSet data);

  ExperimentItem *selectExperiment(nsx::sptrDataSet data);

signals:

  void plotData(nsx::sptrDataSet);

  void inspectWidget(QWidget *);

  void updatePeaks();

  void signalSelectedDataChanged(nsx::sptrDataSet, int frame);

  void signalSelectedPeakChanged(nsx::sptrPeak3D peak);

  void signalEnabledPeakChanged(nsx::sptrPeak3D peak);

  void signalMaskedPeaksChanged(const nsx::PeakList &peaks);

  void signalUnitCellRemoved(nsx::sptrUnitCell unit_cell);

public slots:

  void createNewExperiment();

  void onItemChanged(QStandardItem *item);
};
