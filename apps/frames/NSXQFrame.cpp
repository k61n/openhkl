//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/frames/NSXQFrame.cpp
//! @brief     Implements class NSXQFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "apps/frames/NSXQFrame.h"

NSXQFrame::NSXQFrame() : QFrame(nullptr)
{
    // The frame is destroyed when closed
    setAttribute(Qt::WA_DeleteOnClose, true);

    setWindowFlags(Qt::Window);

    setFrameStyle(QFrame::Box | QFrame::Plain);

    setMouseTracking(true);

    setFocusPolicy(Qt::StrongFocus);
}
