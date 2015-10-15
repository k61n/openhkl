#ifndef CHECKABLECOMBOBOX_H
#define CHECKABLECOMBOBOX_H

#include <QWidget>

#include <QComboBox>


class CheckableComboBox : public QComboBox
{
    Q_OBJECT
public:
    CheckableComboBox(QWidget *parent = 0);
    ~CheckableComboBox();

    virtual void paintEvent(QPaintEvent *);
};




#endif // CHECKABLECOMBOBOX_H
