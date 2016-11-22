#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QComboBox>
#include <QEvent>

class CheckableComboBox : public QComboBox
{
Q_OBJECT
public:
    explicit CheckableComboBox(QWidget *parent = 0);

    void setItemCheckState(int index, Qt::CheckState state);

    QStringList checkedItems() const;

    virtual ~CheckableComboBox();

    const QString& defaultText() const;

    void setDefaultText(const QString& text);

    virtual void hidePopup();

    bool eventFilter(QObject* receiver, QEvent* event);

signals:    

    void checkedItemsChanged(const QStringList& items);

public slots:

    void toggleCheckState(int index);

    void setCheckedItems(const QStringList& items);

    void updateCheckedItems();

private:

    QString _defaultText;

    bool _containerMousePress;

};

#endif // MYCOMBOBOX_H
