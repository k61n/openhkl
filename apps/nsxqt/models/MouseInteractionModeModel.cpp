#include "MouseInteractionModeModel.h"

MouseInteractionModeModel::MouseInteractionModeModel(QObject *parent) : QStandardItemModel(parent)
{
    setRowCount(7);
    setColumnCount(1);
}

QVariant MouseInteractionModeModel::data(const QModelIndex &index, int role) const
{
    auto interaction_mode = static_cast<Mode>(index.row());

    switch (role) {

    case Qt::DecorationRole:
        switch (interaction_mode) {
        case (Mode::SELECTION):
            return QIcon(":/resources/selectIcon.png");
            break;
        case (Mode::ZOOM):
            return QIcon(":/resources/zoomIcon.png");
            break;
        case (Mode::CUTLINE):
            return QIcon(":/resources/cutlineIcon.png");
            break;
        case (Mode::HORIZONTAL_SLICE):
            return QIcon(":/resources/horizontalSliceIcon.png");
            break;
        case (Mode::VERTICAL_SLICE):
            return QIcon(":/resources/verticalSliceIcon.png");
            break;
        case (Mode::RECTANGULAR_MASK):
            return QIcon(":/resources/rectangularMaskIcon.png");
            break;
        case (Mode::ELLIPSOIDAL_MASK):
            return QIcon(":/resources/ellipsoidalMaskIcon.png");
            break;
        default:
            break;
        }
        break;
    case Qt::ToolTipRole:
        switch (interaction_mode) {
        case (Mode::SELECTION):
            return "Selection";
            break;
        case (Mode::ZOOM):
            return "Zoom";
            break;
        case (Mode::CUTLINE):
            return "Cutline";
            break;
        case (Mode::HORIZONTAL_SLICE):
            return "Horizontal slice";
            break;
        case (Mode::VERTICAL_SLICE):
            return "Vertical slice";
            break;
        case (Mode::RECTANGULAR_MASK):
            return "Rectangular mask";
            break;
        case (Mode::ELLIPSOIDAL_MASK):
            return "Ellipsoidal mask";
            break;
        default:
            break;
        }
    default:
        break;
    }

    return QStandardItemModel::data(index,role);
}
