#include <algorithm>
#include <QIcon>

#include "DialogIsotopesDatabase.h"
#include "SampleItem.h"
#include "SamplePropertyWidget.h"
#include "SampleShapeItem.h"

SampleItem::SampleItem() : InspectableTreeItem()
{
    setText("Sample");

    QIcon icon(":/resources/gonioIcon.png");

    setIcon(icon);

    setEditable(false);
    setSelectable(false);
    setDragEnabled(false);
    setDropEnabled(false);

    SampleShapeItem* shape = new SampleShapeItem();
    appendRow(shape);

}

QWidget* SampleItem::inspectItem()
{
    return new SamplePropertyWidget(this);
}

void SampleItem::openIsotopesDatabase()
{
    // dialog will automatically be deleted before we return from this method
    std::unique_ptr<DialogIsotopesDatabase> dialog_ptr(new DialogIsotopesDatabase());

    if (!dialog_ptr->exec()) {
        return;
    }
}
