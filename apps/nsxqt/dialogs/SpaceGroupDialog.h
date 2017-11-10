// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXQT_SPACEGROUPDIALOG_H
#define NSXQT_SPACEGROUPDIALOG_H

#include <memory>
#include <string>
#include <tuple>

#include <Eigen/Dense>

#include <QDialog>

#include <nsxlib/DataSet.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/SpaceGroup.h>

class QModelIndex;
class QWidget;

namespace Ui
{
class SpaceGroupDialog;
}

class SpaceGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpaceGroupDialog(const nsx::PeakSet& peaks, QWidget *parent = 0);
    ~SpaceGroupDialog();

    std::string getSelectedGroup();

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    void evaluateSpaceGroups();
    void buildTable();

    Ui::SpaceGroupDialog *ui;
    nsx::PeakSet _peaks;
    std::vector<std::tuple<std::string, double>> _groups;
    std::set<nsx::sptrUnitCell> _cells;
    std::string _selectedGroup;
};

#endif // NSXQT_SPACEGROUPDIALOG_H
