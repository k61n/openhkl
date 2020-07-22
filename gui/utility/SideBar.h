//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#ifndef NSX_GUI_UTILITY_SIDEBAR_H
#define NSX_GUI_UTILITY_SIDEBAR_H

#include <QToolBar>

class SideBar : public QWidget {
 public:
    SideBar(QWidget* parent = nullptr);
    void addAction(QAction* action);
    QAction* addAction(const QIcon& icon, const QString& text);
    QSize minimumSizeHint() const;
    void manualSelect(int index);

 private:
    void onHome();
    void onExperiment();
    void onFindPeaks();
    void onFilterPeaks();
    void onIndexer();
    void onPredictor();
    void onMerger();

 protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void leaveEvent(QEvent* event);

    QAction* actionAt(const QPoint& at);

 private:
    QList<QAction*> mActions;

    QAction* mCheckedAction;
    QAction* mOverAction;
};

#endif // NSX_GUI_UTILITY_SIDEBAR_H
