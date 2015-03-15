#ifndef SOURCEPROPERTYWIDGET_H
#define SOURCEPROPERTYWIDGET_H

#include <QWidget>

namespace Ui {
class SourcePropertyWidget;
}

class SourceItem;

class SourcePropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SourcePropertyWidget(SourceItem* caller,QWidget *parent = 0);
    ~SourcePropertyWidget();

private:
    SourceItem* _caller;
    Ui::SourcePropertyWidget *ui;
};

#endif // SOURCEPROPERTYWIDGET_H
