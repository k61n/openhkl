//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/properties/UnitCellProperty.cpp
//! @brief     Implements class UnitCellProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/properties/UnitCellProperty.h"

#include "gui/models/Session.h"
#include "base/utils/Units.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"
#include <QCR/engine/cell.h>
#include <QCompleter>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>

UnitCellProperty::UnitCellProperty() : QcrWidget{"unitCellProperty"}
{
    QVBoxLayout* overallLayout = new QVBoxLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    QFormLayout* formLayout = new QFormLayout;
    unitcells = new QcrComboBox("adhoc_unitCellsNames", new QcrCell<int>(0), []() {
        QStringList a{""};
        if (gSession->selectedExperimentNum() < 0)
            return a;
        a += gSession->selectedExperiment()->getUnitCellNames();
        return a;
    });
    unitcells->setHook([=](int i) { selectedCellChanged(i); });
    name = new QcrLineEdit("unitCellName", "");
    QObject::connect(name, &QcrLineEdit::editingFinished, [=]() {
        gSession->selectedExperiment()->getUnitCell()->setName(name->getValue().toStdString());
    });
    spaceGroup = new QcrLineEdit("spaceGroup", "");
    spaceGroup->setReadOnly(true);
    chemicalFormula = new QcrLineEdit("chemicalFormula", "");
    QObject::connect(chemicalFormula, &QcrLineEdit::editingFinished, [=]() {
        if (chemicalFormula->getValue().length() < 1)
            return;
        std::string formula = chemicalFormula->getValue().toStdString();
        gSession->selectedExperiment()->getUnitCell()->setMaterial(
                    std::make_unique<xsection::Material>(formula));
        setMassDensity();
    });
    z = new QcrSpinBox("z", new QcrCell<int>(0), 3);
    indexingTolerance = new QcrDoubleSpinBox("indexingTolerance", new QcrCell<double>(0.00), 6, 4);
    indexingTolerance->setReadOnly(true);
    formLayout->addRow(unitcells);
    formLayout->addRow("Name:", name);
    formLayout->addRow("Spacegroup:", spaceGroup);
    formLayout->addRow("Chemical Formula:", chemicalFormula);
    formLayout->addRow("z", z);
    formLayout->addRow("Indexing Tolerance", indexingTolerance);
    horizontalLayout->addLayout(formLayout);
    horizontalLayout->addItem(
        new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    overallLayout->addLayout(horizontalLayout);

    QGroupBox* cellParameters = new QGroupBox("Cell parameters", this);
    QGridLayout* grid = new QGridLayout(cellParameters);
    a = new QcrDoubleSpinBox("a", new QcrCell<double>(1.0000), 8, 4);
    a->setButtonSymbols(QDoubleSpinBox::NoButtons);
    a->setReadOnly(true);
    b = new QcrDoubleSpinBox("b", new QcrCell<double>(1.0000), 8, 4);
    b->setButtonSymbols(QDoubleSpinBox::NoButtons);
    b->setReadOnly(true);
    c = new QcrDoubleSpinBox("c", new QcrCell<double>(1.0000), 8, 4);
    c->setButtonSymbols(QDoubleSpinBox::NoButtons);
    c->setReadOnly(true);
    alpha = new QcrDoubleSpinBox("alpha", new QcrCell<double>(90.0000), 8, 4);
    alpha->setButtonSymbols(QDoubleSpinBox::NoButtons);
    alpha->setReadOnly(true);
    beta = new QcrDoubleSpinBox("beta", new QcrCell<double>(90.0000), 8, 4);
    beta->setButtonSymbols(QDoubleSpinBox::NoButtons);
    beta->setReadOnly(true);
    gamma = new QcrDoubleSpinBox("gamma", new QcrCell<double>(90.0000), 8, 4);
    gamma->setButtonSymbols(QDoubleSpinBox::NoButtons);
    gamma->setReadOnly(true);
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
    grid->addItem(
        new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 7, 1, 1);
    overallLayout->addWidget(cellParameters);
    setRemake([=]() { });
}

void UnitCellProperty::setZValue(int z)
{
    nsx::sptrUnitCell unit_cell = gSession->selectedExperiment()->getUnitCell();

    unit_cell->setZ(z);
    setMassDensity();
}

void UnitCellProperty::selectedCellChanged(int cell)
{
    if (cell == 0)
        return;
    nsx::sptrUnitCell selectedCell = gSession->selectedExperiment()->getUnitCell(cell-1);
    gSession->selectedExperiment()->selectUnitCell(cell-1);
    name->setCellValue(unitcells->currentText());
    xsection::Material* material = selectedCell->material();
    if (!material)
        chemicalFormula->setCellValue("");
    else
        chemicalFormula->setCellValue(QString::fromStdString(selectedCell->material()->formula()));
    spaceGroup->setCellValue(QString::fromStdString(selectedCell->spaceGroup().symbol()));
    z->setCellValue(selectedCell->z());
    indexingTolerance->setCellValue(selectedCell->indexingTolerance());
    nsx::UnitCellCharacter unitcharacter = selectedCell->character();
    a->setCellValue(unitcharacter.a);
    b->setCellValue(unitcharacter.b);
    c->setCellValue(unitcharacter.c);
    alpha->setCellValue(unitcharacter.alpha);
    beta->setCellValue(unitcharacter.beta);
    gamma->setCellValue(unitcharacter.gamma);
    printAllInformation();
}

void UnitCellProperty::setMassDensity() const
{
    nsx::sptrUnitCell unit_cell = gSession->selectedExperiment()->getUnitCell();

    xsection::Material* material = unit_cell->material();
    if (!material)
        return;

    double mm = material->molarMass();
    mm *= z->value() / nsx::avogadro;
    double volume = unit_cell->volume() * nsx::ang3;
    material->setMassDensity(mm / volume);
}

void UnitCellProperty::printAllInformation()
{
    qDebug() << "Unit Cell Information:";
    nsx::sptrUnitCell cell = gSession->selectedExperiment()->getUnitCell();
    qDebug() << "name: " << QString::fromStdString(cell->name());
    qDebug() << "volume: " << cell->volume();
    qDebug() << "z: " << cell->z();
    xsection::Material* material = cell->material();
    if (!material) {
        qDebug() << "material: none";
        return;
    }
    qDebug() << "material: " << QString::fromStdString(material->formula());
    qDebug() << "molar mass: " << material->molarMass();
    qDebug() << "- - - - - - - - - - - - - - - - - - - - - ";
}
