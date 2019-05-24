
#ifndef NSXGUI_GUI_PROPERTIES_SOURCEPROPERTY_H
#define NSXGUI_GUI_PROPERTIES_SOURCEPROPERTY_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

class SourceProperty : public QcrWidget {
public:
    SourceProperty();

private:
    void onRemake();
    void clear();
    void onMonoChanged(int);
    void onWavelength(double);
    void onWidth(double);
    void onHeight(double);
    void onFwhm(double);
    QcrComboBox* monochromators;
    QcrComboBox* type;
    QcrDoubleSpinBox* wavelength;
    QcrDoubleSpinBox* width;
    QcrDoubleSpinBox* height;
    QcrDoubleSpinBox* fwhm;
};

#endif // NSXGUI_GUI_PROPERTIES_SOURCEPROPERTY_H
