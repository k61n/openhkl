#ifndef NSXGUI_GUI_DIALOGS_LISTNAMEDIALOG_H
#define NSXGUI_GUI_DIALOGS_LISTNAMEDIALOG_H

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

#endif // NSXGUI_GUI_DIALOGS_LISTNAMEDIALOG_H
