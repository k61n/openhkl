//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/frames/FrameInstrumentStates.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "core/experiment/DataTypes.h"

#include "apps/frames/NSXQFrame.h"

namespace Ui {
class FrameInstrumentStates;
}

class QAbstractButton;

class FrameInstrumentStates : public NSXQFrame {
    Q_OBJECT

public:
    static FrameInstrumentStates* create(const nsx::DataList& data);

    static FrameInstrumentStates* Instance();

    ~FrameInstrumentStates();

private slots:

    void slotActionClicked(QAbstractButton* button);

    void slotSelectedDataChanged(int selected_data);

    void slotSelectedFrameChanged(int selected_frame);

private:
    explicit FrameInstrumentStates(const nsx::DataList& data);

private:
    static FrameInstrumentStates* _instance;

    Ui::FrameInstrumentStates* _ui;
};
