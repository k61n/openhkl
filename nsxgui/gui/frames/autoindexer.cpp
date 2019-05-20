
#include "nsxgui/gui/frames/autoindexer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QDialogButtonBox>

AutoIndexer::AutoIndexer()
    : QcrFrame{"autoIndexer"}
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(Qt::Window);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout* whole = new QVBoxLayout(this);
    tabs = new QcrTabWidget("autoIndexerTabs");
    settings = new QcrWidget("Settings");
    QVBoxLayout* vertical = new QVBoxLayout(settings);
    peaks = new QTableView(this);
    vertical->addWidget(peaks);
    QHBoxLayout* horizontal = new QHBoxLayout();
    QGroupBox* params = new QGroupBox("Parameters", this);
    QGridLayout* grid = new QGridLayout(params);
    grid->addWidget(new QLabel("Gruber tolerance"), 0, 0, 1, 1);
    grid->addWidget(new QLabel("Niggli tolerance"), 1, 0, 1, 1);
    only_niggli = new QcrCheckBox("adhoc_onlyNiggli", "find Niggli cell only", new QcrCell<bool>(false));
    grid->addWidget(only_niggli, 2, 0, 1, 1);
    grid->addWidget(new QLabel("maximum cell dimension"), 3, 0, 1, 1);
    grid->addWidget(new QLabel("number Q vertices"), 4, 0, 1, 1);
    grid->addWidget(new QLabel("number subdiversions"), 6, 0, 1, 1);
    grid->addWidget(new QLabel("number solutions"), 5, 0, 1, 1);
    grid->addWidget(new QLabel("minimum cell volume"), 7, 0, 1, 1);
    grid->addWidget(new QLabel("indexing tolerance"), 8, 0, 1, 1);
    gruber = new QcrDoubleSpinBox("adhoc_gruber", new QcrCell<double>(0.04), 8, 6);
    niggli = new QcrDoubleSpinBox("adhoc_niggli", new QcrCell<double>(0.0001), 8, 6);
    maxCellDim = new QcrDoubleSpinBox("adhoc_maxCellDim", new QcrCell<double>(120.00), 6, 2);
    nVertices = new QcrSpinBox("adhoc_vertices", new QcrCell<int>(1000), 4);
    nSolutions = new QcrSpinBox("adhoc_solutions", new QcrCell<int>(10), 4);
    nSubdivisions = new QcrSpinBox("adhoc_subdiversion", new QcrCell<int>(0), 4);
    minCellVolume = new QcrDoubleSpinBox("adhoc_minCellVolume", new QcrCell<double>(0.00), 5, 2);
    indexingTolerance = new QcrDoubleSpinBox("adhoc_indexingTol", new QcrCell<double>(0.200), 5, 3);
    grid->addWidget(gruber, 0, 1, 1, 1);
    grid->addWidget(niggli, 1, 1, 1, 1);
    grid->addWidget(maxCellDim, 3, 1, 1, 1);
    grid->addWidget(nVertices, 4, 1, 1, 1);
    grid->addWidget(nSolutions, 5, 1, 1, 1);
    grid->addWidget(nSubdivisions, 6, 1, 1, 1);
    grid->addWidget(minCellVolume, 7, 1, 1, 1);
    grid->addWidget(indexingTolerance, 8, 1, 1, 1);
    horizontal->addWidget(params);
    solutions = new QTableView(this);
    horizontal->addWidget(solutions);
    vertical->addLayout(horizontal);
    tabs->addTab(settings, "Settings");
    whole->addWidget(tabs);
    QDialogButtonBox* buttons = new QDialogButtonBox(
                QDialogButtonBox::Ok|
                QDialogButtonBox::Apply|
                QDialogButtonBox::Cancel|
                QDialogButtonBox::Reset,
                Qt::Horizontal, this);
    whole->addWidget(buttons);
    show();
}
