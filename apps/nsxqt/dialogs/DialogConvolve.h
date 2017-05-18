// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXQT_DIALOGCONVOLVE_H_
#define NSXQT_DIALOGCONVOLVE_H_

#include <map>
#include <memory>
#include <string>

#include <Eigen/Dense>

#include <QDialog>

#include "ColorMap.h"

class QGraphicsPixmapItem;
class QGraphicsScene;
class QStandardItem;
class QWidget;

namespace nsx {
class PeakFinder;
}

namespace Ui {
class DialogConvolve;
}


class DialogConvolve : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConvolve(const Eigen::MatrixXi& currentFrame,
                            std::shared_ptr<nsx::PeakFinder> peakFinder=nullptr,
                            QWidget *parent = 0);
    ~DialogConvolve();

    void setPreviewFrame(const Eigen::MatrixXi& frame);
    void buildTree();
    void setColorMap(const std::string& name);
    int exec() override;

private slots:
    void on_previewButton_clicked();
    void on_filterComboBox_currentIndexChanged(int index);
    void on_thresholdSpinBox_valueChanged(double arg1);
    void on_confidenceSpinBox_valueChanged(double arg1);
    void on_minCompBox_valueChanged(int arg1);
    void on_maxCompBox_valueChanged(int arg1);
    void on_thresholdComboBox_currentIndexChanged(int index);
    void parameterChanged(QStandardItem* item);

private:
    Ui::DialogConvolve *ui;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _pxmapPreview;
    Eigen::MatrixXi _frame;

    std::shared_ptr<nsx::PeakFinder> _peakFinder;
    std::unique_ptr<ColorMap> _colormap;
};
#endif // NSXQT_DIALOGCONVOLVE_H_
