#include <core/AutoIndexer.h>
#include <core/Logger.h>
#include <core/Peak3D.h>
#include <core/UnitCell.h>
#include <core/Units.h>

#include "CollectedPeaksModel.h"
#include "ExperimentItem.h"
#include "FrameAutoIndexer.h"
#include "UnitCellItem.h"
#include "UnitCellsItem.h"
#include "WidgetUnitCell.h"

#include "ui_FrameAutoIndexer.h"

#include <QInputDialog>

FrameAutoIndexer *FrameAutoIndexer::_instance = nullptr;

FrameAutoIndexer *FrameAutoIndexer::create(ExperimentItem *experiment_item,
                                           const nsx::PeakList &peaks) {
  if (!_instance) {
    _instance = new FrameAutoIndexer(experiment_item, peaks);
  }

  return _instance;
}

FrameAutoIndexer *FrameAutoIndexer::Instance() { return _instance; }

FrameAutoIndexer::FrameAutoIndexer(ExperimentItem *experiment_item,
                                   const nsx::PeakList &peaks)
    : NSXQFrame(), _ui(new Ui::FrameAutoIndexer),
      _experiment_item(experiment_item) {
  _ui->setupUi(this);

  _ui->tabs->tabBar()->tabButton(0, QTabBar::RightSide)->hide();

  // get set of default values to populate controls
  nsx::IndexerParameters params;

  _ui->gruberTolerance->setMinimum(0);
  _ui->gruberTolerance->setMaximum(100);
  _ui->gruberTolerance->setValue(params.gruberTolerance);

  _ui->maxCellDim->setMinimum(0);
  _ui->maxCellDim->setMaximum(9999);
  _ui->maxCellDim->setValue(params.maxdim);

  _ui->nSolutions->setMinimum(3);
  _ui->nSolutions->setMaximum(9999);
  _ui->nSolutions->setValue(params.nSolutions);

  _ui->nVertices->setMinimum(100);
  _ui->nVertices->setMaximum(999999);
  _ui->nVertices->setValue(params.nVertices);

  _ui->subdiv->setMinimum(0);
  _ui->subdiv->setMaximum(999999);
  _ui->subdiv->setValue(params.subdiv);

  _peaks_model = new CollectedPeaksModel(_experiment_item->model(),
                                         _experiment_item->experiment(), peaks);
  _ui->peaks->setModel(_peaks_model);

  _defaults.reserve(peaks.size());
  for (auto peak : peaks) {
    auto unit_cell = peak->unitCell();
    if (unit_cell) {
      _defaults.push_back(std::make_pair(
          peak, std::make_shared<nsx::UnitCell>(nsx::UnitCell(*unit_cell))));
    } else {
      _defaults.push_back(std::make_pair(peak, nullptr));
    }
  }

  connect(_ui->tabs->tabBar(), SIGNAL(tabBarDoubleClicked(int)), this,
          SLOT(slotTabEdited(int)));
  connect(_ui->tabs, SIGNAL(tabCloseRequested(int)), this,
          SLOT(slotTabRemoved(int)));

  connect(_ui->solutions->verticalHeader(), SIGNAL(sectionDoubleClicked(int)),
          this, SLOT(selectSolution(int)));

  connect(_ui->actions, SIGNAL(clicked(QAbstractButton *)), this,
          SLOT(slotActionClicked(QAbstractButton *)));
}

FrameAutoIndexer::~FrameAutoIndexer() {
  if (_peaks_model) {
    delete _peaks_model;
  }

  delete _ui;

  if (_instance) {
    _instance = nullptr;
  }
}

void FrameAutoIndexer::slotTabRemoved(int index) {
  auto unit_cell_tab = dynamic_cast<WidgetUnitCell *>(_ui->tabs->widget(index));
  if (!unit_cell_tab) {
    return;
  }

  _ui->tabs->removeTab(index);

  delete unit_cell_tab;
}

void FrameAutoIndexer::slotTabEdited(int index) {
  auto unit_cell_tab = dynamic_cast<WidgetUnitCell *>(_ui->tabs->widget(index));

  if (!unit_cell_tab) {
    return;
  }

  QInputDialog dialog(this);
  dialog.setLabelText("");
  dialog.setWindowTitle(tr("Set unit cell name"));
  auto pos = mapToGlobal(_ui->tabs->pos());

  int width(0);
  for (auto i = 0; i < index; ++i) {
    width += _ui->tabs->tabBar()->tabRect(index).width();
  }

  int height = _ui->tabs->tabBar()->tabRect(index).height();

  dialog.move(pos.x() + width, pos.y() + height);

  if (dialog.exec() == QDialog::Rejected) {
    return;
  }

  QString unit_cell_name = dialog.textValue();
  if (unit_cell_name.isEmpty()) {
    return;
  }

  _ui->tabs->setTabText(index, unit_cell_name);
  unit_cell_tab->unitCell()->setName(unit_cell_name.toStdString());

  QModelIndex topleft_index = _peaks_model->index(0, 0);
  QModelIndex bottomright_index =
      _peaks_model->index(_peaks_model->rowCount(QModelIndex()) - 1,
                          _peaks_model->columnCount(QModelIndex()) - 1);

  emit _peaks_model->dataChanged(topleft_index, bottomright_index);
}

void FrameAutoIndexer::slotActionClicked(QAbstractButton *button) {
  auto button_role = _ui->actions->standardButton(button);

  switch (button_role) {
  case QDialogButtonBox::StandardButton::Reset: {
    resetUnitCell();
    break;
  }
  case QDialogButtonBox::StandardButton::Apply: {
    run();
    break;
  }
  case QDialogButtonBox::StandardButton::Cancel: {
    resetUnitCell();
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

void FrameAutoIndexer::resetUnitCell() {
  // Restore for each peak the initial unit cell
  for (auto p : _defaults) {
    p.first->setUnitCell(p.second);
  }

  for (auto i = _ui->tabs->count() - 1; i > 0; i--) {

    auto tab = dynamic_cast<WidgetUnitCell *>(_ui->tabs->widget(i));
    if (!tab) {
      continue;
    }
    _ui->tabs->removeTab(i);
    delete tab;
  }

  // Update the peak table view
  QModelIndex topLeft = _peaks_model->index(0, 0);
  QModelIndex bottomRight =
      _peaks_model->index(_peaks_model->rowCount(QModelIndex()) - 1,
                          _peaks_model->columnCount(QModelIndex()) - 1);
  emit _peaks_model->dataChanged(topLeft, bottomRight);
}

void FrameAutoIndexer::accept() {
  auto unit_cells_item = _experiment_item->unitCellsItem();

  for (auto i = 0; i < _ui->tabs->count(); ++i) {
    auto unit_cell_tab = dynamic_cast<WidgetUnitCell *>(_ui->tabs->widget(i));
    if (!unit_cell_tab) {
      continue;
    }
    auto unit_cell_item = new UnitCellItem(unit_cell_tab->unitCell());
    unit_cells_item->appendRow(unit_cell_item);
  }

  emit _experiment_item->model()->itemChanged(unit_cells_item);

  close();
}

void FrameAutoIndexer::run() {
  auto selection_model = _ui->peaks->selectionModel();

  auto selected_rows = selection_model->selectedRows();

  if (selected_rows.empty()) {
    nsx::error() << "No peaks selected for auto-indexing";
    return;
  }

  auto handler = std::make_shared<nsx::ProgressHandler>();

  handler->setCallback([=]() {
    auto log = handler->getLog();
    for (auto &&msg : log) {
      nsx::info() << msg.c_str();
    }
  });

  nsx::AutoIndexer indexer(handler);

  // Clear the current solution list
  _solutions.clear();

  auto peaks = _peaks_model->peaks();

  for (auto r : selected_rows) {
    indexer.addPeak(peaks[r.row()]);
  }

  nsx::IndexerParameters params;

  params.subdiv = _ui->subdiv->value();
  ;
  params.maxdim = _ui->maxCellDim->value();
  params.nSolutions = _ui->nSolutions->value();
  params.indexingTolerance = _ui->indexingTolerance->value();
  params.nVertices = _ui->nVertices->value();
  params.niggliReduction = _ui->niggli_only->isChecked();
  params.niggliTolerance = _ui->niggliTolerance->value();
  params.gruberTolerance = _ui->gruberTolerance->value();
  params.maxdim = _ui->maxCellDim->value();

  try {
    indexer.autoIndex(params);
  } catch (const std::exception &e) {
    nsx::error() << "AutoIndex: " << e.what();
    return;
  }
  _solutions = indexer.solutions();

  buildSolutionsTable();
}

void FrameAutoIndexer::buildSolutionsTable() {
  // Create table with 9 columns
  QStandardItemModel *model =
      new QStandardItemModel(_solutions.size(), 9, this);
  model->setHorizontalHeaderItem(0, new QStandardItem("a"));
  model->setHorizontalHeaderItem(1, new QStandardItem("b"));
  model->setHorizontalHeaderItem(2, new QStandardItem("c"));
  model->setHorizontalHeaderItem(3, new QStandardItem(QString((QChar)0x03B1)));
  model->setHorizontalHeaderItem(4, new QStandardItem(QString((QChar)0x03B2)));
  model->setHorizontalHeaderItem(5, new QStandardItem(QString((QChar)0x03B3)));
  model->setHorizontalHeaderItem(6, new QStandardItem("Volume"));
  model->setHorizontalHeaderItem(7, new QStandardItem("Bravais type"));
  model->setHorizontalHeaderItem(8, new QStandardItem("Quality"));

  using nsx::deg;

  // Display solutions
  for (unsigned int i = 0; i < _solutions.size(); ++i) {
    auto &cell = _solutions[i].first;
    double quality = _solutions[i].second;

    auto ch = cell->character();
    auto sigma = cell->characterSigmas();

    QStandardItem *col1 =
        new QStandardItem(QString::number(ch.a, 'f', 3) + "(" +
                          QString::number(sigma.a * 1000, 'f', 0) + ")");
    QStandardItem *col2 =
        new QStandardItem(QString::number(ch.b, 'f', 3) + "(" +
                          QString::number(sigma.b * 1000, 'f', 0) + ")");
    QStandardItem *col3 =
        new QStandardItem(QString::number(ch.c, 'f', 3) + "(" +
                          QString::number(sigma.c * 1000, 'f', 0) + ")");
    QStandardItem *col4 = new QStandardItem(
        QString::number(ch.alpha / deg, 'f', 3) + "(" +
        QString::number(sigma.alpha / deg * 1000, 'f', 0) + ")");
    QStandardItem *col5 = new QStandardItem(
        QString::number(ch.beta / deg, 'f', 3) + "(" +
        QString::number(sigma.beta / deg * 1000, 'f', 0) + ")");
    QStandardItem *col6 = new QStandardItem(
        QString::number(ch.gamma / deg, 'f', 3) + "(" +
        QString::number(sigma.gamma / deg * 1000, 'f', 0) + ")");
    QStandardItem *col7 =
        new QStandardItem(QString::number(cell->volume(), 'f', 3));
    QStandardItem *col8 =
        new QStandardItem(QString::fromStdString(cell->bravaisTypeSymbol()));
    QStandardItem *col9 =
        new QStandardItem(QString::number(quality, 'f', 2) + "%");

    model->setItem(i, 0, col1);
    model->setItem(i, 1, col2);
    model->setItem(i, 2, col3);
    model->setItem(i, 3, col4);
    model->setItem(i, 4, col5);
    model->setItem(i, 5, col6);
    model->setItem(i, 6, col7);
    model->setItem(i, 7, col8);
    model->setItem(i, 8, col9);
  }
  _ui->solutions->setModel(model);
}

void FrameAutoIndexer::selectSolution(int index) {
  auto selected_unit_cell = _solutions[index].first;

  auto selection_model = _ui->peaks->selectionModel();

  auto selected_rows = selection_model->selectedRows();

  selected_unit_cell->setName("new unit cell");

  auto peaks = _peaks_model->peaks();

  for (auto r : selected_rows) {
    peaks[r.row()]->setUnitCell(selected_unit_cell);
  }

  WidgetUnitCell *widget_unit_cell = new WidgetUnitCell(selected_unit_cell);
  _ui->tabs->addTab(widget_unit_cell,
                    QString::fromStdString(selected_unit_cell->name()));
  QCheckBox *checkbox = new QCheckBox();
  checkbox->setChecked(true);
  _ui->tabs->tabBar()->setTabButton(_ui->tabs->count() - 1, QTabBar::LeftSide,
                                    checkbox);

  QModelIndex topLeft = _peaks_model->index(0, 0);
  QModelIndex bottomRight =
      _peaks_model->index(_peaks_model->rowCount(QModelIndex()) - 1,
                          _peaks_model->columnCount(QModelIndex()) - 1);
  emit _peaks_model->dataChanged(topLeft, bottomRight);
}
