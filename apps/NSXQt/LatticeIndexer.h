#ifndef LATTICEINDEXER_H
#define LATTICEINDEXER_H

#include <QWidget>
#include "UnitCell.h"
#include "Experiment.h"
#include <memory>

namespace Ui {
class LatticeIndexer;
}

class LatticeIndexer : public QWidget
{
    Q_OBJECT

public:
    explicit LatticeIndexer(std::shared_ptr<SX::Crystal::UnitCell> ptrCell,
                            std::shared_ptr<SX::Instrument::Experiment> exp,
                            QWidget *parent = 0);
    void updatePeaks();
    void updateCell();
    ~LatticeIndexer();
private:
    std::shared_ptr<SX::Crystal::UnitCell> _ptrCell;
    std::shared_ptr<SX::Instrument::Experiment> _experiment;
    Ui::LatticeIndexer *ui;

};

#endif // LATTICEINDEXER_H
