//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/widgets/DetectorWidget.h
//! @brief     Defines class DetectorWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_DETECTORWIDGET_H
#define NSX_GUI_UTILITY_DETECTORWIDGET_H

#include "core/data/DataTypes.h"

#include <QGridLayout>

class DetectorScene;
class DetectorView;
class DataComboBox;
class PeakCollectionModel;
class QSpinBox;
class QScrollBar;
class QComboBox;
class QSlider;

class DetectorWidget : public QGridLayout {

 public:
    DetectorWidget(bool mode, bool cursor, bool slider, QWidget* parent = nullptr);

    //! Synchronise intensity sliders and intensities across instances
    void syncIntensitySliders();
    //! Update the DataSet combo
    void updateDatasetList(const std::vector<nsx::sptrDataSet>& data_list);
    //! Refresh the DetectorScene
    void refresh();
    //! Link the detector scene to one or two peak models
    void linkPeakModel(PeakCollectionModel* model1, PeakCollectionModel* model2 = nullptr);
    //! Return the current DataSet
    nsx::sptrDataSet currentData();
    //! Switch the Detectorscene between coordinate types
    void changeView(int option);

    //! Return the DetectorScene
    DetectorScene* scene();
    //! Return the frame QSpinBox
    QSpinBox* spin();
    //! Return the frame QScrollBar
    QScrollBar* scroll();
    //! Return the DataSet QComboBox
    DataComboBox* dataCombo();
    //! Return the interaction mode QComboBox
    QComboBox* modeCombo();
    //! Return the cursor mode QComboBox
    QComboBox* cursorCombo();
    //! Return the maximum intensity slider
    QSlider* slider();
    //! Whether this instance has an intensity slider
    bool hasSlider();

 private:
    DetectorView* _detector_view;
    QSpinBox* _spin;
    QScrollBar* _scroll;
    DataComboBox* _data_combo;
    QComboBox* _mode_combo = nullptr;
    QComboBox* _cursor_combo = nullptr;
    QSlider* _intensity_slider = nullptr;

    bool _has_slider = false;

    static QList<DetectorWidget*> _detector_widgets;
};

#endif // NSX_GUI_UTILITY_DETECTORWIDGET_H
