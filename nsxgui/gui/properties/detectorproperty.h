
#ifndef DETECTORPROPERTY_H
#define DETECTORPROPERTY_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/qcr/widgets/controls.h"
#include <QTableWidget>

class DetectorProperty : public QcrWidget {
public:
	DetectorProperty();
	~DetectorProperty();
private:
	void onValueChanged();
    void onRemake();

	QcrSpinBox* columns;
	QcrSpinBox* rows;
	QcrDoubleSpinBox* height;
	QcrDoubleSpinBox* width;
	QcrDoubleSpinBox* distance;
    QTableWidget* axes;
};

#endif //DETECTORPROPERTY_H
