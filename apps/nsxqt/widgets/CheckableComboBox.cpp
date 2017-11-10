#include <QAbstractItemView>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListView>
#include <QMouseEvent>

#include "CheckableComboBox.h"
#include "CheckableComboBoxModel.h"

CheckableComboBox::CheckableComboBox(QWidget *parent)
: QComboBox(parent),
  _defaultText("Selected items"),
  _containerMousePress(false)
{
    CheckableComboBoxModel* model = new CheckableComboBoxModel();
    setModel(model);

    QListView* view = new QListView();
    setView(view);

    connect(this, SIGNAL(activated(int)), this, SLOT(toggleCheckState(int)));
    connect(this->model(), SIGNAL(checkStateChanged()), this, SLOT(updateCheckedItems()));

    // read-only contents
    QLineEdit* lineEdit = new QLineEdit(this);
    lineEdit->setReadOnly(true);
    setLineEdit(lineEdit);
    lineEdit->disconnect(this);
    setInsertPolicy(QComboBox::NoInsert);

    this->view()->installEventFilter(this);
    this->view()->window()->installEventFilter(this);
    this->view()->viewport()->installEventFilter(this);
    this->installEventFilter(this);
}

CheckableComboBox::~CheckableComboBox()
{
}

void CheckableComboBox::setCheckedItems(const QStringList& items)
{

    for (const auto& item : items)
    {
        const int index = findText(item);
        setItemCheckState(index, index != -1 ? Qt::Checked : Qt::Unchecked);
    }
}

void CheckableComboBox::setItemCheckState(int index, Qt::CheckState state)
{
    setItemData(index, state, Qt::CheckStateRole);
}

QStringList CheckableComboBox::checkedItems() const
{
    QStringList items;
    if (model())
    {
        QModelIndex index = model()->index(0, modelColumn(), rootModelIndex());
        QModelIndexList indexes = model()->match(index, Qt::CheckStateRole, Qt::Checked, -1, Qt::MatchExactly);
        for (const auto& index : indexes)
            items += index.data().toString();
    }
    return items;
}

void CheckableComboBox::updateCheckedItems()
{
    QStringList items = checkedItems();
    if (items.isEmpty())
        setEditText(_defaultText);
    else
        setEditText(items.join(";"));

    emit checkedItemsChanged(items);
}

void CheckableComboBox::toggleCheckState(int index)
{
    QVariant value = itemData(index, Qt::CheckStateRole);
    if (value.isValid())
    {
        Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
        setItemData(index, (state == Qt::Unchecked ? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
    }
}

const QString& CheckableComboBox::defaultText() const
{
    return _defaultText;
}

void CheckableComboBox::setDefaultText(const QString& text)
{
    if (_defaultText != text)
    {
        _defaultText = text;
        updateCheckedItems();
    }
}

void CheckableComboBox::hidePopup()
{
    if (_containerMousePress)
        QComboBox::hidePopup();
}

bool CheckableComboBox::eventFilter(QObject* receiver, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (receiver == this && (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down))
        {
            showPopup();
            return true;
        }
        else if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Escape)
        {
            QComboBox::hidePopup();
            return true;
        }
    }
    case QEvent::MouseButtonPress:
    {
        _containerMousePress = (receiver == view()->window());
        break;
    }
    case QEvent::MouseButtonRelease:
    {
        _containerMousePress = false;
        break;
    }
    default:
        break;
    }
    return false;
}

void CheckableComboBox::addItems(const std::vector<std::string> &items)
{
    for (const auto& item : items)
        addItem(QString::fromStdString(item));
}
