// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXQT_SPACEGROUPDIALOG_H
#define NSXQT_SPACEGROUPDIALOG_H

#include <memory>
#include <string>
#include <tuple>

#include <Eigen/Core>

#include <QDialog>

#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/data/DataTypes.h>

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
    explicit SpaceGroupDialog(nsx::DataList numors, QWidget *parent = 0);
    ~SpaceGroupDialog();

    std::string getSelectedGroup();

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    void evaluateSpaceGroups();
    void buildTable();

    Ui::SpaceGroupDialog *ui;
    nsx::DataList _numors;
    std::vector<std::tuple<std::string, double>> _groups;
    std::string _selectedGroup;
};

#endif // NSXQT_SPACEGROUPDIALOG_H
