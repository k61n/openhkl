//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/InstrumentItem.h
//! @brief     Defines class InstrumentItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

class DetectorItem;
class SampleItem;
class SourceItem;

class InstrumentItem : public TreeItem {
public:
    explicit InstrumentItem(const char* name, const char* source_name);

public:
    DetectorItem* detectorItem();
    SampleItem* sampleItem();
    SourceItem* sourceItem();

private:
    DetectorItem* _detector;
    SampleItem* _sample;
    SourceItem* _source;
};
