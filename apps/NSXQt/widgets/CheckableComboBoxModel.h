#ifndef CHECKABLECOMBOBOXMODEL_H
#define CHECKABLECOMBOBOXMODEL_H

#include <QStandardItemModel>

class CheckableComboBoxModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit CheckableComboBoxModel(QObject *parent = 0);

    virtual Qt::ItemFlags flags(const QModelIndex& index) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

signals:
    void checkStateChanged();

public slots:

};

#endif // CHECKABLECOMBOBOXMODEL_H
