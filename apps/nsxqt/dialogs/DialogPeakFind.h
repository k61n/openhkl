#pragma once

#include <map>
#include <memory>
#include <string>

#include <Eigen/Dense>

#include <QDialog>

#include <nsxlib/DataTypes.h>

#include "ColorMap.h"

class QGraphicsPixmapItem;
class QGraphicsScene;
class QStandardItem;
class QWidget;

namespace Ui {
class DialogPeakFind;
}


class DialogPeakFind : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPeakFind(const nsx::DataList& data,
                            nsx::sptrPeakFinder peakFinder=nullptr,
                            QWidget *parent = 0);
    ~DialogPeakFind();

    void setPreviewFrame(const Eigen::MatrixXi& frame);
    void setColorMap(const std::string& name);
    int exec() override;

private slots:

    void clipPreview(int state);

    void changeThreshold(QString thresold);
    void changeThresholdParameters(int row, int col);

    void changeKernel(QString kernel);
    void changeKernelParameters(int row, int col);

    void changeMinSize(int size);

    void changeMaxSize(int size);

    void changeMaxFrames(int size);

    void changeIntegrationConfidenceValue(double confidence);

    void changeSearchConfidenceValue(double confidence);

    void changeSelectedData(int selected_data);

    void changeSelectedFrame(int selected_frame);

private:

    void buildKernelParametersList();

    void buildThresholdParametersList();

    std::map<std::string,double> getKernelParameters() const;

    std::map<std::string,double> getThresholdParameters() const;

    void updatePreview();

private:
    Ui::DialogPeakFind *ui;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _pxmapPreview;
    nsx::DataList _data;

    nsx::sptrPeakFinder _peakFinder;
    std::unique_ptr<ColorMap> _colormap;
};
