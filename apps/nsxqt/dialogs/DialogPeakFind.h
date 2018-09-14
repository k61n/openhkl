#pragma once

#include <map>
#include <memory>
#include <string>

#include <Eigen/Dense>

#include <QDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QShowEvent>
#include <QStandardItem>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataTypes.h>

#include "ColorMap.h"

class QAbstractButton;

class CollectedPeaksModel;
class ExperimentItem;

namespace Ui {
class DialogPeakFind;
}

class DialogPeakFind : public QDialog
{
    Q_OBJECT

public:

    static DialogPeakFind* create(ExperimentItem* experiment_tree, const nsx::DataList& data, QWidget* parent=nullptr);

    static DialogPeakFind* Instance();

    explicit DialogPeakFind(ExperimentItem *experiment_item, const nsx::DataList& data, QWidget *parent = 0);

    ~DialogPeakFind();

    void setPreviewFrame(const Eigen::MatrixXi& frame);

    void setColorMap(const std::string& name);

    void showEvent(QShowEvent*) override;

    void resizeEvent(QResizeEvent*) override;

public slots:

    virtual void accept() override;

private slots:

    void actionRequested(QAbstractButton* button);

    void clipPreview(int state);

    void changeThreshold();

    void changeConvolver(QString convolver_type);
    void changeConvolverParameters(int row, int col);

    void changeMinSize(int size);
    void changeMaxSize(int size);
    void changeMaxFrames(int size);

    void changeSelectedData(int selected_data);
    void changeSelectedFrame(int selected_frame);

    void find();

private:

    void buildConvolverParametersList();

    std::map<std::string,double> convolverParameters() const;

    void updatePreview();

private:

    static DialogPeakFind *_instance;

    Ui::DialogPeakFind *_ui;

    ExperimentItem *_experiment_item;

    QGraphicsScene *_scene;

    QGraphicsPixmapItem *_pxmapPreview;

    nsx::DataList _data;

    nsx::sptrPeakFinder _peakFinder;

    nsx::PeakList _peaks;

    std::unique_ptr<ColorMap> _colormap;

    CollectedPeaksModel* _peaks_model;
    };
