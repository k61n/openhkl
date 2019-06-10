//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/views/ProgressView.h
//! @brief     Defines class ProgressView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <memory>

#include <QObject>
#include <QProgressDialog>
#include <QTimer>

#include "core/utils/UtilsTypes.h"

class ProgressView : public QProgressDialog {
    Q_OBJECT

public:
    ProgressView(QWidget* parent);
    ~ProgressView();

    void watch(nsx::sptrProgressHandler handler);

public slots:
    void updateProgress();
    void abort();

private:
    nsx::sptrProgressHandler _handler;
    QTimer* _timer;
};
