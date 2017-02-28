// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_SPACEGROUPDIALOG_H_
#define NSXTOOL_SPACEGROUPDIALOG_H_

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QWidget>

#include <QTableWidgetItem>

#include <Eigen/Core>

#include <map>
#include <string>
#include <memory>

#include <nsxlib/data/IData.h>
#include <nsxlib/crystal/SpaceGroup.h>


namespace Ui
{
class SpaceGroupDialog;
}

class SpaceGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpaceGroupDialog(std::vector<std::shared_ptr<SX::Data::DataSet>> numors, QWidget *parent = 0);
    ~SpaceGroupDialog();


    std::string getSelectedGroup();

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    void evaluateSpaceGroups();
    void buildTable();

    Ui::SpaceGroupDialog *ui;
    std::vector<std::shared_ptr<SX::Data::DataSet>> _numors;
    //std::vector<std::tuple<std::string, double, double, double, double>> _groups;
    std::vector<std::tuple<std::string, double>> _groups;
    std::string _selectedGroup;
};
#endif // NSXTOOL_SPACEGROUPDIALOG_H_
