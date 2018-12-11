#include "SamplePropertyWidget.h"

#include <core/Axis.h>
#include <core/Gonio.h>

SamplePropertyWidget::SamplePropertyWidget(const nsx::Sample &sample)
    : QWidget()
    , _ui(new Ui::SamplePropertyWidget)
{
    _ui->setupUi(this);

    const auto &sample_gonio = sample.gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    _ui->axes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _ui->axes->setRowCount(n_sample_gonio_axes);

    _ui->axes->setColumnCount(2);
    _ui->axes->verticalHeader()->setVisible(false);

    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {

        const auto &axis = sample_gonio.axis(i);

        std::ostringstream os;
        os << axis;

        QTableWidgetItem* item0 = new QTableWidgetItem();
        item0->setData(Qt::DisplayRole, QString(axis.name().c_str()));
        item0->setBackgroundColor(axis.physical() ? QColor("#FFDDDD") : QColor("#DDFFDD"));
        _ui->axes->setItem(i,0,item0);

        _ui->axes->setItem(i,1,new QTableWidgetItem(QString(os.str().c_str())));
    }
}

SamplePropertyWidget::~SamplePropertyWidget()
{
    delete _ui;
}
