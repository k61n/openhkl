
#include "nsxgui/gui/panels/subframe_image.h"
#include "nsxgui/gui/mainwin.h"
#include "nsxgui/gui/view/toggles.h"
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! class SubframeImage

SubframeImage::SubframeImage()
    :QcrDockWidget{"Image"}
{
    setWidget(new QTreeView);
    connect(this, SIGNAL( visibilityChanged(bool) ), &gGui->toggles->viewImage,
            SLOT( setChecked(bool)) );
}
