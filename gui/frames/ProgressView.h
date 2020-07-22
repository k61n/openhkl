//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#ifndef NSX_GUI_FRAMES_PROGRESSVIEW_H
#define NSX_GUI_FRAMES_PROGRESSVIEW_H

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

    void watch(nsx::sptrProgressHandler handler);

 public slots:
    void updateProgress();
    void abort();

 private:
    nsx::sptrProgressHandler _handler;
    std::unique_ptr<QTimer> _timer;
};

#endif // NSX_GUI_FRAMES_PROGRESSVIEW_H
