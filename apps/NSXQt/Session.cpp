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

#include "Session.h"

using SX::Instrument::Experiment;
using std::string;
using std::shared_ptr;
using std::pair;

void Session::addExperiment(const string &name, const string &instrumentName)
{
    // experiment already exists; do nothing
    if ( _experiments.find(name) == _experiments.end())
        return;

    _experiments[name] = new Experiment(name, instrumentName);

    // broadcast to all listeners
    emit addExperiment(name, *_experiments[name]);
}

Session::~Session()
{
    for (auto&& expr: _experiments)
        delete expr.second;
}

void Session::addListener(SessionListener *listener)
{
    connect(this, SIGNAL(addExperiment(const string&, const Experiment&)),
            listener, SLOT(addExperiment(const string&, const Experiment&)));
}

void SessionListener::addExperiment(const std::string &name, const Experiment &experiment)
{

}
