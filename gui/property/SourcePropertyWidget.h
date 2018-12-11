#pragma once

#include <core/Source.h>
#include <QWidget>

namespace Ui {
class SourcePropertyWidget;
}

class SourceItem;

class SourcePropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SourcePropertyWidget(nsx::Source& source);

    ~SourcePropertyWidget();

private:
    void onHeightChanged(double height);

    void onSelectedMonochromatorChanged(int index);

    void onWavelengthChanged(double wavelength);

    void onWidthChanged(double width);

private:
    Ui::SourcePropertyWidget *_ui;

    nsx::Source& _source;
};
