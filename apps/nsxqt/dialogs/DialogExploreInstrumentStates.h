#pragma once

#include <QDialog>

#include <nsxlib/DataTypes.h>

class QWidget;

namespace Ui {
class DialogExploreInstrumentStates;
}

class DialogExploreInstrumentStates : public QDialog
{
    Q_OBJECT

public:

    static DialogExploreInstrumentStates* create(const nsx::DataList& data, QWidget *parent=nullptr);

    static DialogExploreInstrumentStates* Instance();

private slots:

    void slotSelectedDataChanged(int selected_data);

    void slotSelectedFrameChanged(int selected_frame);

private:

    DialogExploreInstrumentStates(const nsx::DataList& data, QWidget *parent=nullptr);

private:

    static DialogExploreInstrumentStates* _instance;

    Ui::DialogExploreInstrumentStates* _ui;
};
