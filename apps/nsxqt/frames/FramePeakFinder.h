#pragma  once

#include <map>
#include <memory>
#include <string>

#include <nsxlib/DataTypes.h>

#include "ColorMap.h"
#include "NSXQFrame.h"

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

class FramePeakFinder : public NSXQFrame
{
    Q_OBJECT

public:

    static FramePeakFinder* create(ExperimentItem* experiment_tree, const nsx::DataList& data);

    static FramePeakFinder* Instance();

    FramePeakFinder(ExperimentItem *experiment_item, const nsx::DataList &data);

    virtual ~FramePeakFinder();

    void setColorMap(const std::string &name);

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
    void doActions(QAbstractButton *button);

    //! Change the data for previewing the peak search
    void changePreviewData();

    //! Change the convolution kernel used to perform the peak search
    void changeConvolutionKernel(QString convolution_kernel);
    //! Change the parameters of the convolution kernel used to perform the peak search
    void changeConvolutionParameters(int row, int col);

    //! Clip in the preview the pixels below the selected threshold
    void clipPreview(int state);

    void changeSelectedFrame(int selected_frame);

private:

    void setupLayout();

    void setupConnections();

    void updateConvolutionParameters();

    std::map<std::string,double> convolutionParameters() const;

    void preview();

    void run();

    void accept();

private:

    static FramePeakFinder *_instance;

    ExperimentItem *_experiment_item;

    QGraphicsPixmapItem *_pixmap;

    std::unique_ptr<ColorMap> _colormap;

    //! The threshold spinbox
    QSpinBox *_threshold;

    //! The merge scale spinbox
    QDoubleSpinBox *_merge_scale;

    //! The minimum blob size spinbox
    QSpinBox *_min_blob_size;

    //! The maximum blob size spinbox
    QSpinBox *_max_blob_size;

    //! The blob width spinbox
    QSpinBox *_max_blob_width;

    //! The convolution kernel combox box
    QComboBox *_convolution_kernels;

    //! The convolution kernel parameters table widget
    QTableWidget *_convolution_parameters;

    //! The Data list combox
    QComboBox *_preview_data;

    //! The frame number spinbox
    QSpinBox *_preview_frame_number;

    //! The frame number slider
    QSlider *_preview_frame;

    //! The apply theshold to preview check box
    QCheckBox *_apply_threshold;

    //! The peak area scale spinbox
    QDoubleSpinBox *_peak_scale;

    //! The background lower limit area scale spinbox
    QDoubleSpinBox *_background_begin_scale;

    //! The background upper limit area scale spinbox
    QDoubleSpinBox *_background_end_scale;

    //! The graphics view to show the peak search preview
    QGraphicsView *_preview;

    //! The table that shows the peaks found by the current peak search
    PeakTableView *_peaks;

    //! The button box with the different actions for this dialog
    QDialogButtonBox *_actions;
};
