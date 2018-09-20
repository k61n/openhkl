#pragma once

#include <QDialog>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/InstrumentTypes.h>

namespace Ui
{
class DialogRefiner;
}

class QAbstractButton;
class CollectedPeaksModel;
class ExperimentItem;

class DialogRefiner : public QDialog
{
    Q_OBJECT

public:
    
    static DialogRefiner* create(ExperimentItem* experiment_item, const nsx::PeakList &peaks, QWidget *parent=nullptr);

    static DialogRefiner* Instance();

    ~DialogRefiner();

public slots:

    virtual void accept() override;

private slots:    

    void slotActionClicked(QAbstractButton* button);

private:

    void refine();

private:

    DialogRefiner(ExperimentItem *experiment_item, const nsx::PeakList &peaks, QWidget *parent=nullptr);

    static DialogRefiner *_instance;

    Ui::DialogRefiner *_ui;

    ExperimentItem *_experiment_item;

    CollectedPeaksModel* _peaks_model;
};
