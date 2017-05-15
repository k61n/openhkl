#ifndef LATTICEINDEXER_H
#define LATTICEINDEXER_H

#include <QWidget>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/instrument/Experiment.h>
#include <memory>

namespace Ui {
class LatticeIndexer;
}

class LatticeIndexer : public QWidget
{
    Q_OBJECT

public:
    explicit LatticeIndexer(std::shared_ptr<nsx::Crystal::UnitCell> ptrCell,
                            std::shared_ptr<nsx::Instrument::Experiment> exp,
                            QWidget *parent = 0);
    void updatePeaks();
    void updateCell();
    ~LatticeIndexer();
private:
    std::shared_ptr<nsx::Crystal::UnitCell> _ptrCell;
    std::shared_ptr<nsx::Instrument::Experiment> _experiment;
    Ui::LatticeIndexer *ui;

};

#endif // LATTICEINDEXER_H
