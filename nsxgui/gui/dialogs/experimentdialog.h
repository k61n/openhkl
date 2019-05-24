
#ifndef NSXGUI_GUI_DIALOGS_EXPERIMENTDIALOG_H
#define NSXGUI_GUI_DIALOGS_EXPERIMENTDIALOG_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/modal_dialogs.h>

class ExperimentDialog : public QcrModalDialog {
public:
    ExperimentDialog();
    QString experimentName();
    QString instrumentName();

private:
    QcrComboBox* instruments;
    QcrLineEdit* experiment;
};

#endif // NSXGUI_GUI_DIALOGS_EXPERIMENTDIALOG_H
