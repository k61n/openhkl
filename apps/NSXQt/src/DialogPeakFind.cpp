#include <QDebug>

#include "DialogPeakFind.h"
#include "DialogConvolve.h"



DialogPeakFind::DialogPeakFind(const Eigen::MatrixXi& currentFrame, QWidget *parent):QDialog(parent),ui(new Ui::DialogPeakFind)
{
    ui->setupUi(this);
    setModal(true);
    _currentFrame = currentFrame; // we must be able to forward this to DialogConvolve
}
double DialogPeakFind::getConfidence()
{
    return ui->confidenceSpinBox->value();
}

double DialogPeakFind::getThreshold()
{
    return ui->thresholdSpinBox->value();
}

DialogPeakFind::~DialogPeakFind()
{

}


void DialogPeakFind::on_filterComboBox_activated(int index)
{
    switch(index) {
    // 0: no filter
    case 0:
        return;

    // 1: select filer...
    case 1:
        break;

    // all other indices: unexpected
    default:
        qDebug() << "Unexpected index received from filterComboBox";
        return;
    }

    // execute the convolution filter selection dialog
    DialogConvolve* convolveDialog = new DialogConvolve(_currentFrame, this);
    int retval = convolveDialog->exec();

    // dialog was cancelled or otherwise failed
    if (retval != QDialog::Accepted)
        return;

    // filter was chosen successfully, so update internals to forward to main window
    double threshold = convolveDialog->getThreshold();
    ui->thresholdSpinBox->setValue(threshold);
}
