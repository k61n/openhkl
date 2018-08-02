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

 Forschungszentrum Juelich GmbH
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

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/GeometryTypes.h>
#include <nsxlib/InstrumentTypes.h>
#include <nsxlib/UtilsTypes.h>

class ExperimentItem;

class SessionModel: public QStandardItemModel {
    Q_OBJECT
public:
    explicit SessionModel();
    ~SessionModel();

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    nsx::DataList getSelectedNumors() const;
    nsx::DataList getSelectedNumors(ExperimentItem* item) const;

    void setFilename(QString name);
    QString getFilename();

    void setColorMap(const std::string& name);
    std::string getColorMap() const;

    bool writeXDS(std::string filename, const nsx::PeakList& peaks, bool merge, bool friedel);

    void fitAllPeaks();

    nsx::PeakList peaks(const nsx::DataSet* data) const;

signals:

    void plotData(nsx::sptrDataSet);

    void inspectWidget(QWidget*);

    void updatePeaks();

public slots:

    void createNewExperiment();  

    void onItemChanged(QStandardItem* item);

private:

    QString _filename;

    nsx::sptrProgressHandler _progressHandler;

    std::string _colormap;
};
