/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forshungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXTOOL_SESSION_H_
#define NSXTOOL_SESSION_H_

#include <QObject>

#include <functional>
#include <memory>
#include <string>
#include <map>

#include "Experiment.h"

//! SessionListener class. Any object derived from this class will be notified
//! of any state changes to the corresponding instance of Session
class SessionListener : public QObject {
    Q_OBJECT
public:
    virtual ~SessionListener() {}

signals:

public slots:
    virtual void addExperiment(const std::string& name, const SX::Instrument::Experiment& experiment);

private:
};

//! Session contains the global state of the work session (experiments, data sets, etc.)
//! Any time the state is modified, it broadcasts corresponding signal to all listeners.
class Session : public QObject
{
    Q_OBJECT
public:
    ~Session();

    //! Automatically connect signals and slots
    void addListener(SessionListener* hook);

    void addExperiment(const std::string& name, const std::string& instrumentName);

signals:
    void addExperiment(const std::string& name, const SX::Instrument::Experiment& experiment);

private:
    std::map<std::string, SX::Instrument::Experiment*> _experiments;
};



#endif // NSXTOOL_SESSION_H_
