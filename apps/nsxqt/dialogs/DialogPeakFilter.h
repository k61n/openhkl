#pragma once

#include <map>
#include <string>

#include <QGroupBox>
#include <QDialog>
#include <QVBoxLayout>

#include <nsxlib/crystal/AggregatePeakFilter.h>
#include <nsxlib/crystal/CrystalTypes.h>
#include <nsxlib/data/DataTypes.h>

class QDoubleSpinBox;
class QWidget;

struct FilterGroupBox : public QGroupBox {
    FilterGroupBox(nsx::IPeakFilter* filter, QWidget* parent);
    std::map<std::string,QDoubleSpinBox*> parameterWidgets;

    std::map<std::string,double> parameters() const;
};

class DialogPeakFilter : public QDialog {

public:

    DialogPeakFilter(const nsx::DataList& data, QWidget* parent=0);

    virtual ~DialogPeakFilter()=default;

public slots:

    virtual void accept() override;

private:

    nsx::AggregatePeakFilter _filter;

    nsx::DataList _data;

    std::vector<FilterGroupBox*> _filterGroupBoxes;

};

