
#ifndef NSXGUI_GUI_PROPERTIES_DETECTORPROPERTY_H
#define NSXGUI_GUI_PROPERTIES_DETECTORPROPERTY_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
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

#endif // NSXGUI_GUI_PROPERTIES_DETECTORPROPERTY_H
