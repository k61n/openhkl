//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/frames/ProgressView.h
//! @brief     Defines class ProgressView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_FRAMES_PROGRESSVIEW_H
#define OHKL_GUI_FRAMES_PROGRESSVIEW_H

#include "base/utils/ProgressHandler.h"
#include <QProgressDialog>
#include <memory>

class QTimer;

//! Dialog which shows the current progress of a task
class ProgressView : public QProgressDialog {
    Q_OBJECT

 public:
    ProgressView(QWidget* parent);
    ~ProgressView();

    void watch(ohkl::sptrProgressHandler handler);

 public slots:
    void updateProgress();
    void abort();

 private:
    ohkl::sptrProgressHandler _handler;
    std::unique_ptr<QTimer> _timer;
};

#endif // OHKL_GUI_FRAMES_PROGRESSVIEW_H
