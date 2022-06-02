//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/SpoilerCheck.h
//! @brief     Defines class SubframeFilterPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_SPOILERCHECK_H
#define NSX_GUI_UTILITY_SPOILERCHECK_H

#include "gui/utility/Spoiler.h"

class SpoilerCheck : public Spoiler {

 public:
    explicit SpoilerCheck(const QString& title);
};

#endif // NSX_GUI_UTILITY_SPOILERCHECK_H
