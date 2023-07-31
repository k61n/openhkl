//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/SpoilerCheck.h
//! @brief     Defines class SpoilerCheck
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_UTILITY_SPOILERCHECK_H
#define OHKL_GUI_UTILITY_SPOILERCHECK_H

#include "gui/utility/Spoiler.h"

class SpoilerCheck : public Spoiler {

 public:
    explicit SpoilerCheck(const QString& title);
};

#endif // OHKL_GUI_UTILITY_SPOILERCHECK_H
