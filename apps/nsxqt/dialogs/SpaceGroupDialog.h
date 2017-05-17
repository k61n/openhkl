// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_SPACEGROUPDIALOG_H_
#define NSXTOOL_SPACEGROUPDIALOG_H_

#include <memory>
#include <string>
#include <tuple>

#include <Eigen/Core>

#include <QDialog>

#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/data/IData.h>

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
    explicit SpaceGroupDialog(std::vector<std::shared_ptr<nsx::DataSet>> numors, QWidget *parent = 0);
    ~SpaceGroupDialog();

    std::string getSelectedGroup();

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    void evaluateSpaceGroups();
    void buildTable();

    Ui::SpaceGroupDialog *ui;
    std::vector<std::shared_ptr<nsx::DataSet>> _numors;
    std::vector<std::tuple<std::string, double>> _groups;
    std::string _selectedGroup;
};
#endif // NSXTOOL_SPACEGROUPDIALOG_H_
