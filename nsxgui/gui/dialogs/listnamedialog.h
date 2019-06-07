#ifndef LISTNAMEDIALOG_H
#define LISTNAMEDIALOG_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/modal_dialogs.h>
#include <core/CrystalTypes.h>

class ListNameDialog : public QcrModalDialog {
public:
    ListNameDialog(nsx::PeakList);
    QString listName();
    nsx::PeakList peaklist() { return list_; }

private:
    QcrLineEdit* name_;
    nsx::PeakList list_;
};

#endif // LISTNAMEDIALOG
