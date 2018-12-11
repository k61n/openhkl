#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include <core/PeakFinder.h>

#include "ColorMap.h"
#include "NSXQFrame.h"

namespace Ui {
class FramePeakFinder;
}

namespace nsx {
class ITask;
}

class ExperimentItem;
class PeakTableView;
class QAbstractButton;
class QCheckBox;
class QDialogButtonBox;
class QComboBox;
class QDoubleSpinBox;
class QGraphicsView;
class QGraphicsPixmapItem;
class QSlider;
class QSpinBox;
class QTableWidget;
class QWidget;

class MainWindow;

class FramePeakFinder : public NSXQFrame {
    Q_OBJECT

public:
    static FramePeakFinder*
    create(MainWindow* main_window, ExperimentItem* experiment_tree, const nsx::DataList& data);

    static FramePeakFinder* Instance();

    virtual ~FramePeakFinder();

    void setColorMap(const std::string& name);

    virtual void showEvent(QShowEvent*) override;

    virtual void resizeEvent(QResizeEvent*) override;

private slots:

    //! Update peak finder maximum blob size for a peak
    void changeMaxBlobSize();

    //! Update peak finder minimum blob size for a peak
    void changeMinBlobSize();

    //! Update the peak scale spinbox according to the value of the background begin spinbox
    void changePeakScale();

    //! Update the background begin spinbox according to the value of the background end spinbox
    void changeBackgroundBegin();

    //! Update the background end spinbox according to the value of the background begin spinbox
    void changeBackgroundEnd();

    //! Start one of the actions proposed by the frame actions button box
    void doActions(QAbstractButton* button);

    //! Change the data for previewing the peak search
    void changePreviewData();

    //! Change the convolution kernel used to perform the peak search
    void changeConvolutionKernel(QString convolution_kernel);

    //! Change the parameters of the convolution kernel used to perform the peak search
    void changeConvolutionParameters(int row, int col);

    //! Clip in the preview the pixels below the selected threshold
    void clipPreview(int state);

    void changeSelectedFrame(int selected_frame);

    void slotTabEdited(int index);

    void slotTabRemoved(int index);

private slots:

    void onShowFoundPeaks(std::shared_ptr<nsx::ITask> task);

private:
    FramePeakFinder(
        MainWindow* main_window, ExperimentItem* experiment_item, const nsx::DataList& data);

    void showFoundPeaks(std::shared_ptr<nsx::ITask> task);

    void updateConvolutionParameters();

    std::map<std::string, double> convolutionParameters() const;

    void preview();

    void run();

    void accept();

private:
    static FramePeakFinder* _instance;

    Ui::FramePeakFinder* _ui;

    MainWindow* _main_window;

    ExperimentItem* _experiment_item;

    QGraphicsPixmapItem* _pixmap;

    std::unique_ptr<ColorMap> _colormap;

    std::set<std::shared_ptr<nsx::PeakFinder>> _peak_finders;
};
