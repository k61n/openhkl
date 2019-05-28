
#ifndef SOURCEPROPERTY_H
#define SOURCEPROPERTY_H

#include <QCR/widgets/views.h>
#include <QCR/widgets/controls.h>

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

#endif //SOURCEPROPERTY_H
