#ifndef NSXQT_SAMPLEPROPERTYWIDGET_H
#define NSXQT_SAMPLEPROPERTYWIDGET_H

#include <QWidget>

namespace Ui {
class SamplePropertyWidget;
}

class SampleItem;

class SamplePropertyWidget : public QWidget
{
    Q_OBJECT

public:
    //! Create new SamplePropertyWidget from a Sample TreeItems
    explicit SamplePropertyWidget(SampleItem* caller,QWidget* parent=0);
    ~SamplePropertyWidget();
private slots:
    void cellHasChanged(int,int);
private:
    //!Pointer to the sample
    SampleItem* _sampleItem;
    Ui::SamplePropertyWidget *ui;
};

#endif // NSXQT_SAMPLEPROPERTYWIDGET_H
