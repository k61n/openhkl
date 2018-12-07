#pragma once

#include <string>

#include <QJsonObject>

#include <nsxlib/GeometryTypes.h>
#include <nsxlib/ShapeLibrary.h>

#include "TreeItem.h"

class QWidget;

class LibraryItem : public TreeItem
{
public:

    explicit LibraryItem();

    void incorporateCalculatedPeaks();
    
    nsx::sptrShapeLibrary& library() { return _library; }

private:
    nsx::sptrShapeLibrary _library;
};

