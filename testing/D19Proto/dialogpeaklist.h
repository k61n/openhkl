#ifndef DIALOGPEAKLIST_H
#define DIALOGPEAKLIST_H

#include <QDialog>

namespace Ui {
class DialogPeakList;
}

class DialogPeakList : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPeakList(QWidget *parent = 0);
    ~DialogPeakList();

private:
    Ui::DialogPeakList *ui;
};

#endif // DIALOGPEAKLIST_H
