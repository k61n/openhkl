#include <QCheckBox>
#include <QDebug>

#include "CheckableComboBoxDelegate.h"

CheckableComboBoxDelegate::CheckableComboBoxDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void CheckableComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const bool value = index.data(Qt::UserRole).toBool();
    const QString text = index.data(Qt::DisplayRole).toString();

    const QStyleOptionViewItemV4 sov = static_cast<const QStyleOptionViewItemV4>(option);

    const QStyle *style = sov.widget->style();
    QStyleOptionButton opt;
    opt.state |= value ? QStyle::State_On : QStyle::State_Off;
    opt.state |= QStyle::State_Enabled;
    opt.text = text;
    opt.rect = option.rect;

    style->drawControl(QStyle::CE_CheckBox, &opt,painter);
}

QWidget* CheckableComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QCheckBox *checkBox = new QCheckBox(parent);
    return checkBox;
}

void CheckableComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(editor);
    if (checkBox) {
        const QString text = index.data(Qt::DisplayRole).toString();
        const bool checked = index.data(Qt::UserRole).toBool();
        checkBox->setText(text);
        checkBox->setChecked(checked);
    }
}

void CheckableComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(editor);
    if (checkBox) {
        model->setData(index, checkBox->isChecked(), Qt::UserRole);
    }
}

void CheckableComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
