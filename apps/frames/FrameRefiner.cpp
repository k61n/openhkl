#include <core/Logger.h>
#include <core/Peak3D.h>

#include "CollectedPeaksModel.h"
#include "DoubleItemDelegate.h"
#include "ExperimentItem.h"
#include "FrameRefiner.h"
#include "PeaksItem.h"
#include "SXPlot.h"
#include "WidgetRefinerFit.h"

#include "ui_FrameRefiner.h"

FrameRefiner *FrameRefiner::_instance = nullptr;

FrameRefiner *FrameRefiner::create(ExperimentItem *experiment_item,
                                   const nsx::PeakList &peaks) {
  if (!_instance) {
    _instance = new FrameRefiner(experiment_item, peaks);
  }

  return _instance;
}

FrameRefiner *FrameRefiner::Instance() { return _instance; }

FrameRefiner::FrameRefiner(ExperimentItem *experiment_item,
                           const nsx::PeakList &peaks)
    : NSXQFrame(), _ui(new Ui::FrameRefiner),
      _experiment_item(experiment_item) {
  _ui->setupUi(this);

  _ui->tabs->tabBar()->tabButton(0, QTabBar::RightSide)->hide();

  CollectedPeaksModel *peaks_model = new CollectedPeaksModel(
      _experiment_item->model(), _experiment_item->experiment(), peaks);
  _ui->peaks->setModel(peaks_model);
  _ui->peaks->selectAll();

  _ui->tabs->setCurrentIndex(0);

  connect(_ui->tabs, SIGNAL(tabCloseRequested(int)), this,
          SLOT(slotTabRemoved(int)));

  connect(_ui->actions, SIGNAL(clicked(QAbstractButton *)), this,
          SLOT(slotActionClicked(QAbstractButton *)));
}

FrameRefiner::~FrameRefiner() {
  delete _ui;

  if (_instance) {
    _instance = nullptr;
  }
}

void FrameRefiner::slotTabRemoved(int index) {
  auto refiner_fit_tab =
      dynamic_cast<WidgetRefinerFit *>(_ui->tabs->widget(index));
  if (!refiner_fit_tab) {
    return;
  }

  _ui->tabs->removeTab(index);

  delete refiner_fit_tab;
}

void FrameRefiner::slotActionClicked(QAbstractButton *button) {
  auto button_role = _ui->actions->standardButton(button);

  switch (button_role) {
  case QDialogButtonBox::StandardButton::Apply: {
    refine();
    break;
  }
  case QDialogButtonBox::StandardButton::Cancel: {
    close();
    break;
  }
  case QDialogButtonBox::StandardButton::Ok: {
    accept();
    break;
  }
  default: { return; }
  }
}

void FrameRefiner::refine() {
  // Check and construct the peak selection
  auto selection_model = _ui->peaks->selectionModel();
  QModelIndexList selected_rows = selection_model->selectedRows();
  if (selected_rows.size() < 100) {
    nsx::error() << "No or not enough peaks selected for refining";
    return;
  }

  auto peaks_model = dynamic_cast<CollectedPeaksModel *>(_ui->peaks->model());
  auto &&peaks = peaks_model->peaks();
  nsx::PeakList selected_peaks;
  for (auto r : selected_rows) {
    selected_peaks.push_back(peaks[r.row()]);
  }

  auto unit_cell = selected_peaks[0]->unitCell();

  if (!unit_cell) {
    nsx::error() << "No unit cell set for the selected peaks";
    return;
  }

  auto &&n_batches = _ui->n_batches->value();

  std::map<nsx::sptrDataSet, nsx::Refiner> refiners;

  std::set<nsx::sptrDataSet> data;
  // get list of datasets
  for (auto p : peaks) {
    data.insert(p->data());
  }

  for (auto d : data) {

    nsx::PeakList reference_peaks, predicted_peaks;

    // Keep the peak that belong to this data and split them between the found
    // and predicted ones
    for (auto peak : selected_peaks) {
      if (peak->data() != d) {
        continue;
      }
      if (peak->predicted()) {
        predicted_peaks.push_back(peak);
      } else {
        reference_peaks.push_back(peak);
      }
    }

    nsx::info() << reference_peaks.size() << " splitted into " << n_batches
                << "refining batches.";

    std::vector<nsx::InstrumentState> &states = d->instrumentStates();

    nsx::Refiner refiner(states, unit_cell, reference_peaks, n_batches);

    if (_ui->refine_ub->isChecked()) {
      refiner.refineUB();
      nsx::info() << "Refining UB matrix";
    }

    if (_ui->refine_sample_position->isChecked()) {
      refiner.refineSamplePosition();
      nsx::info() << "Refinining sample position";
    }

    if (_ui->refine_detector_position->isChecked()) {
      refiner.refineDetectorOffset();
      nsx::info() << "Refinining detector position";
    }

    if (_ui->refine_sample_orientation->isChecked()) {
      refiner.refineSampleOrientation();
      nsx::info() << "Refinining sample orientation";
    }

    if (_ui->refine_ki->isChecked()) {
      refiner.refineKi();
      nsx::info() << "Refining Ki";
    }

    bool success = refiner.refine();

    if (success) {
      nsx::info() << "Successfully refined parameters for numor "
                  << d->filename();
      int updated = refiner.updatePredictions(predicted_peaks);
      refiners.emplace(d, std::move(refiner));
      nsx::info() << "done; updated " << updated << " peaks";
    } else {
      nsx::info() << "Failed to refine parameters for numor " << d->filename();
    }
  }

  if (!refiners.empty()) {
    if (_ui->tabs->count() == 2) {
      auto refiner_fit_tab = _ui->tabs->widget(1);
      _ui->tabs->removeTab(1);
      delete refiner_fit_tab;
    }
    WidgetRefinerFit *refiner_fit_tab = new WidgetRefinerFit(refiners);
    _ui->tabs->addTab(refiner_fit_tab, "Fit");
  }

  // Update the peak table view
  QModelIndex topLeft = peaks_model->index(0, 0);
  QModelIndex bottomRight =
      peaks_model->index(peaks_model->rowCount(QModelIndex()) - 1,
                         peaks_model->columnCount(QModelIndex()) - 1);
  emit peaks_model->dataChanged(topLeft, bottomRight);
}

void FrameRefiner::accept() {
  auto peaks_item = _experiment_item->peaksItem();

  emit _experiment_item->model()->itemChanged(peaks_item);

  close();
}
