
#include "nsxgui/gui/properties/unitcellproperty.h"

#include <QCR/engine/cell.h>
#include "nsxgui/gui/models/session.h"
#include <QGroupBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCompleter>
#include <core/SpaceGroup.h>
#include <core/UnitCell.h>

UnitCellProperty::UnitCellProperty()
    : QcrWidget{"unitCellProperty"}
{
    QVBoxLayout* overallLayout = new QVBoxLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    QFormLayout* formLayout = new QFormLayout;
    name = new QcrLineEdit("unitCellName", "");
    QStringList spacegroups;
    for (auto &&sg : nsx::SpaceGroup::symbols()) {
      spacegroups.append(QString::fromStdString(sg));
    }
    spaceGroup = new QcrComboBox("spaceGroup", new QcrCell<int>(0), spacegroups);
    QCompleter *completer =
        new QCompleter(spaceGroup->model(), spaceGroup);
    spaceGroup->setCompleter(completer);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseSensitive);
    chemicalFormula = new QcrLineEdit("chemicalFormula", "");
    z = new QcrSpinBox("z", new QcrCell<int>(0), 3);
    indexingTolerance = new QcrDoubleSpinBox("indexingTolerance", new QcrCell<double>(0.00), 6, 4);
    formLayout->addRow("Name:", name);
    formLayout->addRow("Spacegroup:", spaceGroup);
    formLayout->addRow("Chemical Formula:", chemicalFormula);
    formLayout->addRow("z", z);
    formLayout->addRow("Indexing Tolerance", indexingTolerance);
    horizontalLayout->addLayout(formLayout);
    horizontalLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    overallLayout->addLayout(horizontalLayout);

    QGroupBox* cellParameters = new QGroupBox("Cell parameters", this);
    QGridLayout* grid = new QGridLayout(cellParameters);
    a = new QcrDoubleSpinBox("a", new QcrCell<double>(1.0000), 8, 4);
    b = new QcrDoubleSpinBox("b", new QcrCell<double>(1.0000), 8, 4);
    c = new QcrDoubleSpinBox("c", new QcrCell<double>(1.0000), 8, 4);
    alpha = new QcrDoubleSpinBox("alpha", new QcrCell<double>(90.0000), 8, 4);
    beta = new QcrDoubleSpinBox("beta", new QcrCell<double>(90.0000), 8, 4);
    gamma = new QcrDoubleSpinBox("gamma", new QcrCell<double>(90.0000), 8, 4);
    grid->addWidget(a, 0, 2, 1, 1);
    grid->addWidget(b, 0, 4, 1, 1);
    grid->addWidget(c, 0, 6, 1, 1);
    grid->addWidget(alpha, 1, 2, 1, 1);
    grid->addWidget(beta, 1, 4, 1, 1);
    grid->addWidget(gamma, 1, 6, 1, 1);
    grid->addWidget(new QLabel("a"), 0, 0, 1, 1);
    grid->addWidget(new QLabel("b"), 0, 3, 1, 1);
    grid->addWidget(new QLabel("c"), 0, 5, 1, 1);
    grid->addWidget(new QLabel(QString((QChar)0x03B1)), 1, 0, 1, 1);
    grid->addWidget(new QLabel(QString((QChar)0x03B2)), 1, 3, 1, 1);
    grid->addWidget(new QLabel(QString((QChar)0x03B3)), 1, 5, 1, 1);
    grid->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 7, 1, 1);
    overallLayout->addWidget(cellParameters);
}

void UnitCellProperty::setZValue(int z)
{
    auto unit_cell = gSession->selectedExperiment()->unitCells()->selectedCell();

    unit_cell->setZ(z);
    //setMassDensity();
}
