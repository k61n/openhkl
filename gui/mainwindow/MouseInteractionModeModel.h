#pragma once

#include <QObject>
#include <QStandardItemModel>

class MouseInteractionModeModel : public QStandardItemModel
{
    Q_OBJECT

public:

    enum class Mode {SELECTION,ZOOM,CUTLINE,HORIZONTAL_SLICE,VERTICAL_SLICE,RECTANGULAR_MASK,ELLIPSOIDAL_MASK};

    MouseInteractionModeModel(QObject *parent);

    QVariant data(const QModelIndex &index, int role) const override;
};
