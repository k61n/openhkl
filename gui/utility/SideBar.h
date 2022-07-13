//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/SideBar.h
//! @brief     Defines class SideBar
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_UTILITY_SIDEBAR_H
#define OHKL_GUI_UTILITY_SIDEBAR_H

#include <QToolBar>
#include <QToolTip>
#include <array>

// subclass Action to integrate tooltips

class SideBar : public QWidget {
    Q_OBJECT
 public:
    SideBar(QWidget* parent = nullptr);
    void addAction(QAction* action);
    QAction* addAction(const QIcon& icon, const QString& text);
    QSize minimumSizeHint() const;
    void manualSelect(int index);
    void refreshAll();

    void refreshCurrent();

 signals:
    void subframeChanged();

 private:
    void onHome();
    void onExperiment();
    void onFindPeaks();
    void onFilterPeaks();
    void onIndexer();
    void onShapes();
    void onPredictor();
    void onRefiner();
    void onIntegrator();
    void onMerger();

 protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void leaveEvent(QEvent* event);

    QAction* actionAt(const QPoint& at);

 private:
    QList<QAction*> mActions;

    std::array<QIcon, 9> mEnabledIcons;
    std::array<QIcon, 9> mDisabledIcons;

    QAction* mCheckedAction;
    QAction* mOverAction;

    const unsigned int _min_icon_height = 80;
};

#endif // OHKL_GUI_UTILITY_SIDEBAR_H
