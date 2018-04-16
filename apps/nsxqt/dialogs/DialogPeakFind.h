#pragma once

#include <map>
#include <memory>
#include <string>

#include <Eigen/Dense>

#include <QDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QStandardItem>
#include <QShowEvent>

#include <nsxlib/DataTypes.h>

#include "ColorMap.h"

namespace Ui {
class DialogPeakFind;
}

class DialogPeakFind : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPeakFind(const nsx::DataList& data,nsx::sptrPeakFinder peakFinder=nullptr,QWidget *parent = 0);
    ~DialogPeakFind();

    void setPreviewFrame(const Eigen::MatrixXi& frame);
    void setColorMap(const std::string& name);
    int exec() override;
    void showEvent(QShowEvent*);
    double peakScale() const;
    double bkgBegin() const;
    double bkgEnd() const;

private slots:

    void clipPreview(int state);

    void changeThreshold(QString thresold_type);
    void changeThresholdParameters(int row, int col);

    void changeConvolver(QString convolver_type);
    void changeConvolverParameters(int row, int col);

    void changeMinSize(int size);
    void changeMaxSize(int size);
    void changeMaxFrames(int size);

    void changeSelectedData(int selected_data);
    void changeSelectedFrame(int selected_frame);

private:

    void buildConvolverParametersList();

    void buildThresholdParametersList();

    std::map<std::string,double> convolverParameters() const;

    std::map<std::string,double> thresholdParameters() const;

    void updatePreview();

private:
    Ui::DialogPeakFind *ui;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _pxmapPreview;
    nsx::DataList _data;

    nsx::sptrPeakFinder _peakFinder;
    std::unique_ptr<ColorMap> _colormap;
};
