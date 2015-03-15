#ifndef SOURCEPROPERTWIDGET_H
#define SOURCEPROPERTWIDGET_H

#include <QWidget>

namespace Ui {
class SourcePropertWidget;
}

class SourceItem;

class SourcePropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SourcePropertyWidget(SourceItem* caller,QWidget *parent = 0);
    ~SourcePropertyWidget();

private:
    Ui::SourcePropertWidget *ui;
    SourceItem* _caller;
};

#endif // SOURCEPROPERTWIDGET_H
