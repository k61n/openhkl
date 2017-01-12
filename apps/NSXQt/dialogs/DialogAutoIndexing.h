#ifndef DIALOGAUTOINDEXING_H
#define DIALOGAUTOINDEXING_H

#include <QDialog>

#include <memory>
#include <vector>

#include "Peak3D.h"
#include "UnitCell.h"

namespace Ui
{
class DialogAutoIndexing;
}

namespace SX
{
namespace Instrument
{
class Experiment;
}
namespace Crystal
{
class Peak3D;
}
}

using SX::Instrument::Experiment;
using SX::Crystal::UnitCell;
using SX::Crystal::sptrPeak3D;

class DialogAutoIndexing : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAutoIndexing(std::shared_ptr<Experiment> experiment, QWidget *parent = 0);
    ~DialogAutoIndexing();

    void buildSolutionsTable();

public slots:

    void autoIndex();


private:
    Ui::DialogAutoIndexing *ui;

    std::shared_ptr<Experiment> _experiment;

    std::vector<sptrPeak3D> _peaks;

    std::vector<std::pair<UnitCell,double>> _solutions;

};

#endif // DIALOGAUTOINDEXING_H
