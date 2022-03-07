//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subwindows/InstrumentStateWindow.cpp
//! @brief     Implemets class InstrumentStateWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subwindows/InstrumentStateWindow.h"

#include "gui/MainWin.h" // gGui
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/LinkedComboBox.h"

#include <QVBoxLayout>
#include <QSignalBlocker>

InstrumentStateWindow::InstrumentStateWindow(QWidget* parent)
    : QDialog(parent)
{
    auto* main_layout = new QVBoxLayout(this);

    _data_combo = new LinkedComboBox(ComboType::DataSet, gGui->sentinel);
    main_layout->addWidget(_data_combo);

    refreshAll();
}

void InstrumentStateWindow::updateData()
{
    if (gSession->numExperiments() == 0)
        return;

    const QSignalBlocker blocker(this);
    QString current_data = _data_combo->currentText();
    _data_combo->clear();

    const QStringList& datanames{gSession->currentProject()->getDataNames()};
    if (!datanames.empty()) {
        _data_combo->addItems(datanames);
        _data_combo->setCurrentText(current_data);
    }
}


void InstrumentStateWindow::refreshAll()
{
    updateData();
}
