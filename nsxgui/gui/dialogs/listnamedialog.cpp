
#include "nsxgui/gui/dialogs/listnamedialog.h"
#include "nsxgui/gui/mainwin.h"
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

ListNameDialog::ListNameDialog(nsx::PeakList list)
    : QcrModalDialog{"adhoc_ListNameDialog", gGui, "List name dialog"}
    , list_{list}
{
    resize(500, 130);
    setMinimumSize(500,130);
    setMaximumSize(500, 130);

    QVBoxLayout* whole = new QVBoxLayout(this);
    QHBoxLayout* line = new QHBoxLayout;
    name_ = new QcrLineEdit("adhoc_listName", "");
    line->addWidget(new QLabel("Peaklist name: "));
    line->addWidget(name_);
    whole->addLayout(line);
    QDialogButtonBox* buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok|
                                                       QDialogButtonBox::Cancel,
                                                       Qt::Horizontal);
    whole->addWidget(buttonbox);

    QObject::connect(buttonbox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonbox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ListNameDialog::listName()
{
    if (name_->getValue() == "")
        return "not named peaklist";
    return name_->getValue();
}
