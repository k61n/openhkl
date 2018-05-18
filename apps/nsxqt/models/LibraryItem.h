#pragma once

#include <string>

#include <QJsonObject>

#include <nsxlib/GeometryTypes.h>

#include "TreeItem.h"

class QWidget;

class LibraryItem : public TreeItem
{
public:

    explicit LibraryItem();
    void incorporateCalculatedPeaks();

private:
    nsx::sptrShapeLibrary _library;
};

