#ifndef NSXQT_LATTICEINDEXER_H
#define NSXQT_LATTICEINDEXER_H

#include <memory>

#include <QWidget>

#include <nsxlib/crystal/CrystalTypes.h>
#include <nsxlib/instrument/InstrumentTypes.h>

namespace Ui {
class LatticeIndexer;
}

class LatticeIndexer : public QWidget {
    Q_OBJECT

public:
    explicit LatticeIndexer(nsx::sptrUnitCell cell,
                            nsx::sptrExperiment exp,
                            QWidget *parent = 0);
    void updatePeaks();
    void updateCell();
    ~LatticeIndexer();
private:
    nsx::sptrUnitCell _cell;
    nsx::sptrExperiment _experiment;
    Ui::LatticeIndexer *ui;

};

#endif // NSXQT_LATTICEINDEXER_H
