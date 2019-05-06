
#include "gui/panels/subframe_image.h"
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! class SubframeImage

SubframeImage::SubframeImage()
    :QDockWidget{"Image"}
{
    setWidget(new QTreeView);
}
