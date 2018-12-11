#include "DetectorPropertyWidget.h"

#include <QDoubleSpinBox>
#include <QSpinBox>

#include <core/Diffractometer.h>
#include <core/DirectVector.h>
#include <core/Gonio.h>
#include <core/RotAxis.h>
#include <core/TransAxis.h>

DetectorPropertyWidget::DetectorPropertyWidget(nsx::Detector& detector)
    : QWidget(), _ui(new Ui::DetectorPropertyWidget), _detector(detector)
{
    _ui->setupUi(this);

    const auto& detector_gonio = _detector.gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();

    _ui->height->setValue(_detector.height());
    _ui->width->setValue(_detector.width());
    _ui->columns->setValue(_detector.nCols());
    _ui->rows->setValue(_detector.nRows());
    _ui->sample_to_detector_distance->setValue(_detector.distance());

    _ui->axes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _ui->axes->setRowCount(n_detector_gonio_axes);

    _ui->axes->setColumnCount(2);
    _ui->axes->verticalHeader()->setVisible(false);

    for (size_t i = 0; i < n_detector_gonio_axes; ++i) {

        const auto& axis = detector_gonio.axis(i);

        std::ostringstream os;
        os << axis;

        QTableWidgetItem* item0 = new QTableWidgetItem();
        item0->setData(Qt::DisplayRole, QString(axis.name().c_str()));
        item0->setBackgroundColor(axis.physical() ? QColor("#FFDDDD") : QColor("#DDFFDD"));
        _ui->axes->setItem(i, 0, item0);

        _ui->axes->setItem(i, 1, new QTableWidgetItem(QString(os.str().c_str())));
    }

    connect(
        _ui->sample_to_detector_distance,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [=](double value) { onSampleToDectorDistanceChanged(value); });
}

DetectorPropertyWidget::~DetectorPropertyWidget()
{
    delete _ui;
}

void DetectorPropertyWidget::onSampleToDectorDistanceChanged(double distance)
{
    _detector.setDistance(distance);
}
