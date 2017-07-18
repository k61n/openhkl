#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include <nsxlib/crystal/PeakFilterFactory.h>
#include <nsxlib/data/DataSet.h>

#include "DialogPeakFilter.h"

FilterGroupBox::FilterGroupBox(nsx::IPeakFilter* filter, QWidget* parent) : QGroupBox(parent)
{
    setCheckable(true);
    setTitle(QString::fromStdString(filter->description()));
    QGridLayout* layout = new QGridLayout(this);
    const auto& parameters = filter->parameters();
    unsigned int parameter_comp = 0;
    for (auto&& parameter : parameters) {
        layout->addWidget(new QLabel(QString::fromStdString(parameter.first),this), 0, parameter_comp++);
        QDoubleSpinBox* parameter_value = new QDoubleSpinBox(this);
        parameter_value->setValue(parameter.second);
        layout->addWidget(parameter_value, 0, parameter_comp++);
        parameterWidgets.insert(std::make_pair(parameter.first,parameter_value));
    }
}

std::map<std::string,double> FilterGroupBox::parameters() const
{
    std::map<std::string,double> parameters;

    for (auto w : parameterWidgets) {
        parameters.insert(std::make_pair(w.first,w.second->value()));
    }
    return parameters;
}

DialogPeakFilter::DialogPeakFilter(const nsx::DataList& data, QWidget* parent)
: QDialog(parent),
  _data(data)
{
    QVBoxLayout* main_layout = new QVBoxLayout(this);

    nsx::PeakFilterFactory* peakFilterFactory = nsx::PeakFilterFactory::Instance();

    auto filter_names = peakFilterFactory->list();

    for (auto filter_name : filter_names) {
        nsx::IPeakFilter* peak_filter = peakFilterFactory->create(filter_name);
        _filter.addFilter(*peak_filter);
        FilterGroupBox* group_box = new FilterGroupBox(peak_filter,this);
        _filterGroupBoxes.push_back(group_box);
        main_layout->addWidget(group_box);
        delete peak_filter;
    }

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    main_layout->addWidget(button_box);

    this->setLayout(main_layout);

    connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));
}

void DialogPeakFilter::accept()
{
    auto&& filters = _filter.filters();

    for (auto i=0; i< filters.size(); ++i) {

        bool is_activated = _filterGroupBoxes[i]->isChecked();
        filters[i]->setActivated(is_activated);
        if (is_activated) {
            filters[i]->setParameters(_filterGroupBoxes[i]->parameters());
        }
    }

    for (nsx::sptrDataSet data : _data) {

        data->removeDuplicatePeaks();

        nsx::PeakSet& peaks = data->getPeaks();

        auto&& filtered_peaks = _filter.filter(peaks);

        for (auto peak : filtered_peaks) {
            peaks.erase(peak);
        }
    }

    QDialog::accept();
}
