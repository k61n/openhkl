#pragma once

#include <string>
#include <Eigen/Dense>
#include <QDialog>

#include <core/PeakList.h>
#include <core/UnitCell.h>

class QModelIndex;
class QWidget;

namespace Ui
{
class DialogSpaceGroup;
}

class DialogSpaceGroup : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSpaceGroup(const nsx::PeakList& peaks, QWidget *parent = 0);
    ~DialogSpaceGroup();

    std::string getSelectedGroup();

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    void evaluateSpaceGroups();
    void buildTable();

    Ui::DialogSpaceGroup *ui;
    nsx::PeakList _peaks;
    std::vector<std::pair<std::string, double>> _groups;
    nsx::sptrUnitCell _cell;
    std::string _selectedGroup;
};
