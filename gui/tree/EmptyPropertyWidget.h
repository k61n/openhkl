#pragma once

#include <QWidget>

namespace Ui {
class EmptyPropertyWidget;
}

class EmptyPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EmptyPropertyWidget(QWidget *parent = 0);
    ~EmptyPropertyWidget();

private:
    Ui::EmptyPropertyWidget *ui;
};
