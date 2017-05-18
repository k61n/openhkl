#ifndef NSXQT_LATTICEINDEXER_H
#define NSXQT_LATTICEINDEXER_H

#include <memory>

#include <QWidget>

#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/instrument/Experiment.h>

namespace Ui {
class LatticeIndexer;
}

class LatticeIndexer : public QWidget {
    Q_OBJECT

public:
    explicit LatticeIndexer(std::shared_ptr<nsx::UnitCell> ptrCell,
                            std::shared_ptr<nsx::Experiment> exp,
                            QWidget *parent = 0);
    void updatePeaks();
    void updateCell();
    ~LatticeIndexer();
private:
    std::shared_ptr<nsx::UnitCell> _ptrCell;
    std::shared_ptr<nsx::Experiment> _experiment;
    Ui::LatticeIndexer *ui;

};

#endif // NSXQT_LATTICEINDEXER_H
