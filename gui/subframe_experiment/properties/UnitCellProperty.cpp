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

#include <QCompleter>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

UnitCellProperty::UnitCellProperty() : QWidget()
{
    setSizePolicies();
    setSizePolicy(*_size_policy_box);

    QVBoxLayout* overallLayout = new QVBoxLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    QGridLayout* grid_layout = new QGridLayout;

    QLabel* label_ptr;

    label_ptr = new QLabel("Name:");
    label_ptr->setAlignment(Qt::AlignRight);
    grid_layout->addWidget(label_ptr, 1, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Space group:");
    label_ptr->setAlignment(Qt::AlignRight);
    grid_layout->addWidget(label_ptr, 2, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Chemical formula:");
    label_ptr->setAlignment(Qt::AlignRight);
    grid_layout->addWidget(label_ptr, 3, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Z:");
    label_ptr->setAlignment(Qt::AlignRight);
    grid_layout->addWidget(label_ptr, 4, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Indexing tolerance:");
    label_ptr->setAlignment(Qt::AlignRight);
    grid_layout->addWidget(label_ptr, 5, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    unitcells = new QComboBox();
    name = new QLineEdit();
    spaceGroup = new QLineEdit();
    chemicalFormula = new QLineEdit();
    z = new QSpinBox();
    indexingTolerance = new QDoubleSpinBox();
    a = new QDoubleSpinBox();
    b = new QDoubleSpinBox();
    c = new QDoubleSpinBox();
    alpha = new QDoubleSpinBox();
    beta = new QDoubleSpinBox();
    gamma = new QDoubleSpinBox();
    
    unitcells->setSizePolicy(*_size_policy_widgets);

    name->setSizePolicy(*_size_policy_widgets);

    spaceGroup->setReadOnly(true);
    spaceGroup->setSizePolicy(*_size_policy_widgets);

    indexingTolerance->setReadOnly(true);
    indexingTolerance->setSizePolicy(*_size_policy_widgets);

    chemicalFormula->setSizePolicy(*_size_policy_widgets);

    z->setSizePolicy(*_size_policy_widgets);

    a->setButtonSymbols(QDoubleSpinBox::NoButtons);
    a->setSizePolicy(*_size_policy_widgets);
    a->setReadOnly(true);
    a->setDecimals(5);

    b->setButtonSymbols(QDoubleSpinBox::NoButtons);
    b->setSizePolicy(*_size_policy_widgets);
    b->setReadOnly(true);
    b->setDecimals(5);

    c->setButtonSymbols(QDoubleSpinBox::NoButtons);
    c->setSizePolicy(*_size_policy_widgets);
    c->setReadOnly(true);
    c->setDecimals(5);

    alpha->setButtonSymbols( QDoubleSpinBox::NoButtons);
    alpha->setSizePolicy(*_size_policy_widgets);
    alpha->setReadOnly(true);
    alpha->setDecimals(5);

    beta->setButtonSymbols(QDoubleSpinBox::NoButtons);
    beta->setSizePolicy(*_size_policy_widgets);
    beta->setReadOnly(true);
    beta->setDecimals(5);

    gamma->setButtonSymbols(QDoubleSpinBox::NoButtons);
    gamma->setSizePolicy(*_size_policy_widgets);
    gamma->setReadOnly(true);
    gamma->setDecimals(5);

    grid_layout->addWidget(unitcells, 0, 0, 1, 2);
    grid_layout->addWidget(name, 1, 1, 1, 1);
    grid_layout->addWidget(spaceGroup, 2, 1, 1, 1);
    grid_layout->addWidget(chemicalFormula, 3, 1, 1, 1);
    grid_layout->addWidget(z, 4, 1, 1, 1);
    grid_layout->addWidget(indexingTolerance, 5, 1, 1, 1);

    horizontalLayout->addLayout(grid_layout);

    overallLayout->addLayout(horizontalLayout);

    QGroupBox* cellParameters = new QGroupBox("Cell parameters", this);
    QGridLayout* grid = new QGridLayout(cellParameters);

    grid->addWidget(a, 0, 1, 1, 1);
    grid->addWidget(b, 1, 1, 1, 1);
    grid->addWidget(c, 2, 1, 1, 1);
    grid->addWidget(alpha, 0, 3, 1, 1);
    grid->addWidget(beta, 1, 3, 1, 1);
    grid->addWidget(gamma, 2, 3, 1, 1);

    grid->addWidget(new QLabel("a (A):"), 0, 0, 1, 1);
    grid->addWidget(new QLabel("b (A):"), 1, 0, 1, 1);
    grid->addWidget(new QLabel("c (A):"), 2, 0, 1, 1);
    grid->addWidget(new QLabel(QString((QChar)0x03B1)), 0, 2, 1, 1);
    grid->addWidget(new QLabel(QString((QChar)0x03B2)), 1, 2, 1, 1);
    grid->addWidget(new QLabel(QString((QChar)0x03B3)), 2, 2, 1, 1);

    overallLayout->addWidget(cellParameters);
    overallLayout->addStretch();

    connect(
        unitcells, static_cast<void (QComboBox::*) (int) >(&QComboBox::currentIndexChanged),
        this, &UnitCellProperty::selectedCellChanged
    );

    connect(
        name, &QLineEdit::editingFinished, 
        [=]() {
            std::string old_name = unitcells->currentText().toStdString();
            std::string new_name = name->text().toStdString();

            if (old_name == new_name)
                return;

            gSession->selectedExperiment()->experiment()->addUnitCell(
                new_name,gSession->selectedExperiment()->experiment()->getUnitCell(old_name));
            gSession->selectedExperiment()->experiment()->getUnitCell(
                new_name)->setName(new_name);
            gSession->selectedExperiment()->experiment()->swapUnitCells(
                old_name,new_name);    
            gSession->selectedExperiment()->experiment()->removeUnitCell(old_name);

            refreshInput();

            int idx =  gSession->selectedExperiment()->getUnitCellNames().indexOf(QString::fromStdString(new_name));
            unitcells->setCurrentIndex(idx);

            printAllInformation();
    });

    connect(chemicalFormula, &QLineEdit::editingFinished, [=]() {
        if (chemicalFormula->text().length() < 1)
            return;
        std::string formula = chemicalFormula->text().toStdString();
        gSession->selectedExperiment()->experiment()->getUnitCell(unitcells->currentText().toStdString())->setMaterial(
                    std::make_unique<xsection::Material>(formula));
        setMassDensity();
        printAllInformation();
    });
}

void UnitCellProperty::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);
    
    _size_policy_box = new QSizePolicy();
    _size_policy_box->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_box->setVerticalPolicy(QSizePolicy::Preferred);

    _size_policy_right = new QSizePolicy();
    _size_policy_right->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_right->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_fixed = new QSizePolicy();
    _size_policy_fixed->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_fixed->setVerticalPolicy(QSizePolicy::Fixed);
}

void UnitCellProperty::refreshInput()
{
    unitcells->blockSignals(true);
    unitcells->clear();
    unitcells->addItems(gSession->selectedExperiment()->getUnitCellNames());
    unitcells->blockSignals(false);

    if (!gSession->selectedExperiment()->getUnitCellNames().isEmpty())
        selectedCellChanged(0);
}

void UnitCellProperty::setZValue(int z)
{
    nsx::UnitCell* unit_cell = gSession->selectedExperiment()->experiment()->getUnitCell(unitcells->currentText().toStdString());

    unit_cell->setZ(z);
    setMassDensity();
}

void UnitCellProperty::selectedCellChanged(int cell)
{
    if (gSession->selectedExperiment()->experiment()->getUnitCellNames().size() == 0)
        return;

    nsx::UnitCell* selected_cell = gSession->selectedExperiment()->experiment()->getUnitCell(gSession->selectedExperiment()->experiment()->getUnitCellNames()[cell]);

    name->setText(QString::fromStdString(selected_cell->name()));
    xsection::Material* material = selected_cell->material();
    if (!material)
        chemicalFormula->setText(QString::fromStdString(""));
    else
        chemicalFormula->setText(QString::fromStdString(selected_cell->material()->formula()));
    spaceGroup->setText(QString::fromStdString(selected_cell->spaceGroup().symbol()));
    z->setValue(selected_cell->z());
    indexingTolerance->setValue(selected_cell->indexingTolerance());
    nsx::UnitCellCharacter unitcharacter = selected_cell->character();
    a->setValue(unitcharacter.a);
    b->setValue(unitcharacter.b);
    c->setValue(unitcharacter.c);
    alpha->setValue(unitcharacter.alpha);
    beta->setValue(unitcharacter.beta);
    gamma->setValue(unitcharacter.gamma);
    printAllInformation();
}

void UnitCellProperty::setMassDensity() const
{
    nsx::UnitCell* unit_cell = gSession->selectedExperiment()->experiment()->getUnitCell(unitcells->currentText().toStdString());

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
    nsx::UnitCell* cell = gSession->selectedExperiment()->experiment()->getUnitCell(unitcells->currentText().toStdString());
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
