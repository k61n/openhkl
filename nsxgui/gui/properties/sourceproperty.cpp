
#include "nsxgui/gui/properties/sourceproperty.h"
#include "nsxgui/qcr/engine/cell.h"
#include "nsxgui/gui/models/session.h"
#include <core/Units.h>
#include <QGridLayout>

SourceProperty::SourceProperty()
  : QcrWidget("sourceProperty")
{
    QGridLayout* grid = new QGridLayout(this);

    type = new QcrComboBox("sourcetype", new QcrCell<int>(0), {"Neutron"});
    type->setEnabled(false);

    monochromators = new QcrComboBox("monochromators", new QcrCell<int>(0), [this](){
                                         QStringList list;
                                         ExperimentModel* exp = gSession->selectedExperiment();
                                         if (!exp)
                                             return list;
                                         const auto &monos = exp->experiment()->diffractometer()->
                                         source().monochromators();
                                         for (auto&& m : monos) {
                                             list.append(QString::fromStdString(m.name()));
                                         }
                                         return list;
                                     });
    monochromators->setEnabled(true);

    wavelength = new QcrDoubleSpinBox("wavelength", new QcrCell<double>(0.00), 5, 2);

    fwhm = new QcrDoubleSpinBox("fwhm", new QcrCell<double>(0.00), 5, 2);

    width = new QcrDoubleSpinBox("width", new QcrCell<double>(0.00), 5, 2);

    height = new QcrDoubleSpinBox("height", new QcrCell<double>(0.00), 5, 2);

    grid->addWidget(type, 0, 1, 1, 1);
    grid->addWidget(monochromators, 1, 1, 1, 1);
    grid->addWidget(wavelength, 2, 1, 1, 1);
    grid->addWidget(fwhm, 3, 1, 1, 1);
    grid->addWidget(width, 4, 1, 1, 1);
    grid->addWidget(height, 5, 1, 1, 1);

    grid->addWidget(new QLabel("Type:"), 0, 0, 1, 1);
    grid->addWidget(new QLabel("Monochromators:"), 1, 0, 1, 1);
    grid->addWidget(new QLabel("Wavelength (ang):"), 2, 0, 1, 1);
    grid->addWidget(new QLabel("FWHM (ang):"), 3, 0, 1, 1);
    grid->addWidget(new QLabel("Width (mm):"), 4, 0, 1, 1);
    grid->addWidget(new QLabel("Height (mm):"), 5, 0, 1, 1);
    grid->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 6, 3, 1, 1);

    setRemake([this](){ onRemake(); });
    remake();
}

void SourceProperty::onRemake()
{
    clear();
    monochromators->remake();
    if (gSession->selectedExperimentNum()>=0)
        onMonoChanged(0);
}

void SourceProperty::clear()
{
    wavelength->setCellValue(0.00);
    fwhm->setCellValue(0.00);
    width->setCellValue(0.00);
    height->setCellValue(0.00);
}

void SourceProperty::onMonoChanged(int index)
{
    auto &source = gSession->selectedExperiment()->experiment()->diffractometer()->source();
    source.setSelectedMonochromator(index);

    const auto &mono = source.selectedMonochromator();

    wavelength->setCellValue(mono.wavelength());
    fwhm->setCellValue(mono.fullWidthHalfMaximum());
    height->setCellValue(mono.height() / nsx::mm);
    width->setCellValue(mono.width() / nsx::mm);
}
