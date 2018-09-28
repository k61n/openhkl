#pragma  once

#include <array>

#include <nsxlib/CrystalTypes.h>

#include "NSXQFrame.h"

class QAbstractButton;
class ExperimentItem;
class PeakTableView;
class QCheckBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QListWidget;
class QSlider;
class QSpinBox;

class FrameRefiner : public NSXQFrame
{
    Q_OBJECT

public:

    static FrameRefiner* create(ExperimentItem* experiment_tree, const nsx::PeakList & peaks);

    static FrameRefiner* Instance();

    FrameRefiner(ExperimentItem *experiment_item, const nsx::PeakList &peaks);

    virtual ~FrameRefiner();

private slots:

    //! Start one of the actions proposed by the frame actions button box
    void doActions(QAbstractButton *button);

private:

    void setupLayout();

    void setupConnections();

    void run();

    void accept();

private:

    static FrameRefiner *_instance;

    ExperimentItem *_experiment_item;

    //! The table that shows the peaks usable for the refining
    PeakTableView *_peaks;

    //! The checkbox for refining or not the UB matrix
    QCheckBox *_refine_ub;

    //! The checkbox for refining or not the sample_position
    QCheckBox *_refine_sample_position;

    //! The checkbox for refining or not the sample_orientation
    QCheckBox *_refine_sample_orientation;

    //! The checkbox for refining or not the incident beam
    QCheckBox *_refine_ki;

    //! The checkbox for refining or not the detector offsets
    QCheckBox *_refine_detector_offsets;

    //! The spinbox to set the number of frames per batch
    QSpinBox *_nframes_per_batch;

    //! The button box with the different actions for this dialog
    QDialogButtonBox *_actions;

    //! The list widget for storing the data for which we want to inspect the offsets
    QListWidget *_datas;

    //! The spin box for selecting data for which we want to inspect the offsets
    QSpinBox *_selected_frame;

    //! The slider for selecting data for which we want to inspect the offsets
    QSlider *_selected_frame_slider;

    //! The double spin boxes for viewing the offset for sample position
    std::array<QDoubleSpinBox*,3> _sample_position;

    //! The double spin boxes for viewing the sample orientation
    std::array<std::array<QDoubleSpinBox*,3>,3> _sample_orientation;

    //! The double spin boxes for viewing the sample orientation offsets
    std::array<std::array<QDoubleSpinBox*,3>,3> _sample_orientation_offsets;

    //! The double spin boxes for viewing the detector orientation offsets
    std::array<std::array<QDoubleSpinBox*,3>,3> _detector_orientation;

    //! The double spin box for viewing the incoment beam
    std::array<QDoubleSpinBox*,3> _ni;
};
