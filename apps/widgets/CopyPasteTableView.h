//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/widgets/CopyPasteTableView.h
//! @brief     Defines class CopyPasteTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QKeyEvent>
#include <QTableView>

class CopyPasteTableView : public QTableView {
public:
    CopyPasteTableView(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event);
};
