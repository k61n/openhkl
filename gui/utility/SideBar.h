//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/SideBar.h
//! @brief     Defines class SideBar
//!
//! @homepage  https://openhkl.org
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

enum class SubFrame {
    Home = 0,
    Experiment,
    Find,
    Filter,
    Index,
    Shapes,
    Predict,
    Refine,
    Integrate,
    Reject,
    Merge,
    Rescale
};

class SideBar : public QWidget {
    Q_OBJECT
 public:
    SideBar(QWidget* parent = nullptr);
    void addAction(QAction* action);
    QAction* addAction(const QIcon& icon, const QString& text);
    QSize minimumSizeHint() const;
    void manualSelect(int index);
    void refreshAll();
    void setStrategyMode(bool strategy) { _strategy = strategy; };

    void refreshCurrent();

 public slots:
    void onSubframeChanged();

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
    void onReject();
    void onMerger();
    void onRescale();

 protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void leaveEvent(QEvent* event);

    QAction* actionAt(const QPoint& at);

 private:
    QList<QAction*> mActions;

    std::array<QIcon, 10> mEnabledIcons;
    std::array<QIcon, 10> mDisabledIcons;

    QAction* mCheckedAction;
    QAction* mOverAction;

    bool _strategy;

    const unsigned int _min_icon_height = 60;
};

#endif // OHKL_GUI_UTILITY_SIDEBAR_H
