
#ifndef NSXGUI_GUI_FRAMES_GLOBALOFFSETS_H
#define NSXGUI_GUI_FRAMES_GLOBALOFFSETS_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTableWidget>

enum class offsetMode { DETECTOR, SAMPLE };

class GlobalOffsets : public QcrFrame {
public:
    GlobalOffsets(offsetMode mode);

private:
    void layout();
    void fit();
    void actionClicked(QAbstractButton* button);

    QcrSpinBox* iterations;
    QcrDoubleSpinBox* tolerance;
    QListWidget* selectedData;
    QTableWidget* offsets;
    QDialogButtonBox* buttons;
    offsetMode mode_;
};

#endif // NSXGUI_GUI_FRAMES_GLOBALOFFSETS_H
