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
class LinkedComboBox;
class PeakCollectionModel;
class QSpinBox;
class QScrollBar;
class QComboBox;
class QSlider;

class DetectorWidget : public QGridLayout {

 public:
    DetectorWidget(bool mode, bool cursor, bool slider, QWidget* parent = nullptr);

    void updateDatasetList(const std::vector<nsx::sptrDataSet>& data_list);
    void refresh();
    void linkPeakModel(PeakCollectionModel* model1, PeakCollectionModel* model2 = nullptr);
    nsx::sptrDataSet currentData();
    void changeView(int option);

    DetectorScene* scene();
    QSpinBox* spin();
    QScrollBar* scroll();
    LinkedComboBox* dataCombo();
    QComboBox* modeCombo();
    QComboBox* cursorCombo();
    QSlider* slider();

 private:
    DetectorView* _detector_view;
    QSpinBox* _spin;
    QScrollBar* _scroll;
    LinkedComboBox* _data_combo = nullptr;
    QComboBox* _mode_combo = nullptr;
    QComboBox* _cursor_combo = nullptr;
    QSlider* _intensity_slider = nullptr;

    std::vector<nsx::sptrDataSet> _data_list;
};

#endif // NSX_GUI_UTILITY_DETECTORWIDGET_H
