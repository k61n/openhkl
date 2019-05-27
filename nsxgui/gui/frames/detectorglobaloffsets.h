
#ifndef DETECTORGLOBALOFFSETS_H
#define DETECTORGLOBALOFFSETS_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/qcr/widgets/controls.h"
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTableWidget>

class DetectorGlobalOffsets : public QcrFrame {
public:
	DetectorGlobalOffsets();
private:
	void layout();
    void fit();
    void actionClicked(QAbstractButton* button);

	QcrSpinBox* iterations;
	QcrDoubleSpinBox* tolerance;
	QListWidget* selectedData;
	QTableWidget* offsets;
	QDialogButtonBox* buttons;
};

#endif //DETECTORGLOBALOFFSETS_H
