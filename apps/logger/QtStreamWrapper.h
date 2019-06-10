//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/logger/QtStreamWrapper.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

//@ #include <sstream>
//@ #include <string>

//@ #include <QObject>

#include "core/logger/IStreamWrapper.h"

class NoteBook;

class QtStreamWrapper : public QObject, public nsx::IStreamWrapper {

    Q_OBJECT

public:
    //! Constructor
    QtStreamWrapper(
        NoteBook* notebook, std::function<std::string()> prefix = nullptr,
        std::function<std::string()> suffix = nullptr);

    //! Destructor
    virtual ~QtStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message) override;

    virtual void printPrefix() override;

    virtual void printSuffix() override;

signals:

    void sendLogMessage(const std::string& message);

private:
    std::ostringstream _os;
};
