#include "NSXQFrame.h"

NSXQFrame::NSXQFrame() : QFrame(nullptr)
{
    // The frame is destroyed when closed
    setAttribute(Qt::WA_DeleteOnClose, true);

    setWindowFlags(Qt::Window);

    setFrameStyle(QFrame::Box | QFrame::Plain);

    setMouseTracking(true);

    setFocusPolicy(Qt::StrongFocus);
}
