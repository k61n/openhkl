//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/progressview.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_FRAMES_PROGRESSVIEW_H
#define GUI_FRAMES_PROGRESSVIEW_H

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

#endif // GUI_FRAMES_PROGRESSVIEW_H
