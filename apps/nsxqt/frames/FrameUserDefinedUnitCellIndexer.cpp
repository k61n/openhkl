#include <QStandardItem>
#include <QStandardItemModel>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Monochromator.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/Source.h>
#include <nsxlib/UserDefinedUnitCellIndexer.h>
#include <nsxlib/Units.h>

#include "CollectedPeaksModel.h"
#include "ExperimentItem.h"
#include "FrameUserDefinedUnitCellIndexer.h"
#include "PeaksItem.h"
#include "SessionModel.h"

#include "ui_FrameUserDefinedUnitCellIndexer.h"

FrameUserDefinedUnitCellIndexer* FrameUserDefinedUnitCellIndexer::_instance = nullptr;

FrameUserDefinedUnitCellIndexer* FrameUserDefinedUnitCellIndexer::create(ExperimentItem *experiment_item, const nsx::PeakList &peaks)
{
    if (!_instance) {
        _instance = new FrameUserDefinedUnitCellIndexer(experiment_item, peaks);
    }

    return _instance;
}

FrameUserDefinedUnitCellIndexer* FrameUserDefinedUnitCellIndexer::Instance()
{
    return _instance;
}

FrameUserDefinedUnitCellIndexer::FrameUserDefinedUnitCellIndexer(ExperimentItem* experiment_item, const nsx::PeakList &peaks)
: NSXQFrame(),
  _ui(new Ui::FrameUserDefinedUnitCellIndexer),
  _experiment_item(experiment_item),
  _indexer()
{
    _ui->setupUi(this);

    CollectedPeaksModel *peaks_model = new CollectedPeaksModel(_experiment_item->model(),_experiment_item->experiment(),peaks);
    _ui->peaks->setModel(peaks_model);
    _ui->peaks->selectAll();

    _ui->label_a->setText("a ("+ QString(QChar(0x212B)) + ")");
    _ui->label_b->setText("b ("+ QString(QChar(0x212B)) + ")");
    _ui->label_c->setText("c ("+ QString(QChar(0x212B)) + ")");

    _ui->label_alpha->setText(QString(QChar(0x03B1)) + "(" + QString(QChar(0x00B0)) + ")");
    _ui->label_beta->setText(QString(QChar(0x03B2)) + "(" + QString(QChar(0x00B0)) + ")");
    _ui->label_gamma->setText(QString(QChar(0x03B3)) + "(" + QString(QChar(0x00B0)) + ")");

    _ui->label_wavelength->setText("Wavelength ("+ QString(QChar(0x212B)) + ")");

    auto&& parameters = _indexer.parameters();

    _ui->a->setValue(parameters.a);
    _ui->b->setValue(parameters.b);
    _ui->c->setValue(parameters.c);

    _ui->alpha->setValue(parameters.alpha/nsx::deg);
    _ui->beta->setValue(parameters.beta/nsx::deg);
    _ui->gamma->setValue(parameters.gamma/nsx::deg);

    _ui->distance_tolerance->setValue(parameters.distance_tolerance);

    _ui->angular_tolerance->setValue(parameters.angular_tolerance);

    _ui->niggli_tolerance->setValue(parameters.niggli_tolerance);
    _ui->gruber_tolerance->setValue(parameters.gruber_tolerance);
    _ui->niggli_only->setChecked(parameters.niggli_only);

    _ui->n_solutions->setValue(parameters.n_solutions);
    _ui->indexing_tolerance->setValue(parameters.indexing_tolerance);
    _ui->indexing_threshold->setValue(parameters.indexing_threshold);

    _ui->wavelength->setValue(_experiment_item->experiment()->diffractometer()->source()->selectedMonochromator().wavelength());

    _ui->max_n_q_vectors->setValue(parameters.max_n_q_vectors);

    connect(_ui->actions,SIGNAL(clicked(QAbstractButton*)),this,SLOT(slotActionClicked(QAbstractButton*)));
}

FrameUserDefinedUnitCellIndexer::~FrameUserDefinedUnitCellIndexer()
{
    delete _ui;

    if (_instance) {
        _instance = nullptr;
    }
}

void FrameUserDefinedUnitCellIndexer::slotActionClicked(QAbstractButton *button)
{
    auto button_role = _ui->actions->standardButton(button);

    switch(button_role)
    {
    case QDialogButtonBox::StandardButton::Apply: {
        index();
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
    default: {
        return;
    }
    }
}

void FrameUserDefinedUnitCellIndexer::index()
{
    auto selection_model = _ui->peaks->selectionModel();

    auto selected_rows = selection_model->selectedRows();

    if (selected_rows.empty()) {
        nsx::error()<<"No peaks selected for auto-indexing";
        return;
    }

    auto peaks_model = dynamic_cast<CollectedPeaksModel*>(_ui->peaks->model());
    auto peaks = peaks_model->peaks();

    nsx::PeakList selected_peaks;
    selected_peaks.reserve(selected_rows.size());
    for (auto r : selected_rows) {
        selected_peaks.push_back(peaks[r.row()]);
    }

    nsx::UserDefinedUnitCellIndexerParameters parameters;

    parameters.a = _ui->a->value();
    parameters.b = _ui->b->value();
    parameters.c = _ui->c->value();

    parameters.alpha = _ui->alpha->value() * nsx::deg;
    parameters.beta = _ui->beta->value() * nsx::deg;
    parameters.gamma = _ui->gamma->value() * nsx::deg;

    parameters.niggli_tolerance = _ui->niggli_tolerance->value();
    parameters.gruber_tolerance = _ui->gruber_tolerance->value();
    parameters.niggli_only = _ui->niggli_only->isChecked();

    parameters.n_solutions = _ui->n_solutions->value();
    parameters.indexing_tolerance = _ui->indexing_tolerance->value();
    parameters.indexing_threshold = _ui->indexing_threshold->value();

    parameters.wavelength = _ui->wavelength->value();

    parameters.distance_tolerance = _ui->distance_tolerance->value();

    parameters.angular_tolerance = _ui->angular_tolerance->value();

    parameters.max_n_q_vectors = _ui->max_n_q_vectors->value();

    _indexer.setParameters(parameters);

    _indexer.setPeaks(selected_peaks);

    _indexer.run();

    _solutions = _indexer.solutions();

    buildUnitCellsTable();
}

void FrameUserDefinedUnitCellIndexer::buildUnitCellsTable()
{
    // Create table with 9 columns
    QStandardItemModel* model = new QStandardItemModel(_solutions.size(),9,this);
    model->setHorizontalHeaderItem(0,new QStandardItem("a"));
    model->setHorizontalHeaderItem(1,new QStandardItem("b"));
    model->setHorizontalHeaderItem(2,new QStandardItem("c"));
    model->setHorizontalHeaderItem(3,new QStandardItem(QString((QChar) 0x03B1)));
    model->setHorizontalHeaderItem(4,new QStandardItem(QString((QChar) 0x03B2)));
    model->setHorizontalHeaderItem(5,new QStandardItem(QString((QChar) 0x03B3)));
    model->setHorizontalHeaderItem(6,new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(7,new QStandardItem("Bravais type"));
    model->setHorizontalHeaderItem(8,new QStandardItem("Quality"));

    // Display solutions
    for (size_t i = 0; i < _solutions.size(); ++i) {

        auto&& unit_cell = _solutions[i].first;

        const double quality = _solutions[i].second;

        auto ch = unit_cell->character();
        auto sigma = unit_cell->characterSigmas();

        QStandardItem* col1 = new QStandardItem(QString::number(ch.a,'f',3) + "("+ QString::number(sigma.a*1000,'f',0)+")");
        QStandardItem* col2 = new QStandardItem(QString::number(ch.b,'f',3) + "("+ QString::number(sigma.b*1000,'f',0)+")");
        QStandardItem* col3 = new QStandardItem(QString::number(ch.c,'f',3) + "("+ QString::number(sigma.c*1000,'f',0)+")");
        QStandardItem* col4 = new QStandardItem(QString::number(ch.alpha/nsx::deg,'f',3)+ "("+ QString::number(sigma.alpha/nsx::deg*1000,'f',0)+")");
        QStandardItem* col5 = new QStandardItem(QString::number(ch.beta/nsx::deg,'f',3)+"("+ QString::number(sigma.beta/nsx::deg*1000,'f',0)+")");
        QStandardItem* col6 = new QStandardItem(QString::number(ch.gamma/nsx::deg,'f',3)+ "("+ QString::number(sigma.gamma/nsx::deg*1000,'f',0)+")");
        QStandardItem* col7 = new QStandardItem(QString::number(unit_cell->volume(),'f',3));
        QStandardItem* col8 = new QStandardItem(QString::fromStdString(unit_cell->bravaisTypeSymbol()));
        QStandardItem* col9 = new QStandardItem(QString::number(quality,'f',2)+"%");

        model->setItem(i,0,col1);
        model->setItem(i,1,col2);
        model->setItem(i,2,col3);
        model->setItem(i,3,col4);
        model->setItem(i,4,col5);
        model->setItem(i,5,col6);
        model->setItem(i,6,col7);
        model->setItem(i,7,col8);
        model->setItem(i,8,col9);
    }

    _ui->unit_cells->setModel(model);
}

void FrameUserDefinedUnitCellIndexer::accept()
{
    auto peaks_item = _experiment_item->peaksItem();

    emit _experiment_item->model()->itemChanged(peaks_item);

    close();
}
