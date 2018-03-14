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
class DialogConvolve;
}


class DialogConvolve : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConvolve(const nsx::DataList& data,
                            nsx::sptrPeakFinder peakFinder=nullptr,
                            QWidget *parent = 0);
    ~DialogConvolve();

    void setPreviewFrame(const Eigen::MatrixXi& frame);
    void setColorMap(const std::string& name);
    int exec() override;

private slots:

    void changeConvolutionParameters(QStandardItem* item);

    void changeThresholdType(int index);

    void changeBlobMinSize(int size);

    void changeBlobMaxSize(int size);

    void changeSearchScale(double scale);
    void changeIntegrationScale(double scale);
    void changeBackgroundScale(double scale);

    void changeThresholdValue(double threshold);

    void changeConvolutionFilter(int filter);

    void changeSelectedData(int selected_data);

    void changeSelectedFrame(int selected_frame);

private:

    void buildConvolutionParametersList();

    void updatePreview();

private:
    Ui::DialogConvolve *ui;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _pxmapPreview;
    nsx::DataList _data;

    nsx::sptrPeakFinder _peakFinder;
    std::unique_ptr<ColorMap> _colormap;
};
