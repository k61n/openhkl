#pragma once

#include <QKeyEvent>
#include <QTableView>

class CopyPasteTableView : public QTableView {
public:
    CopyPasteTableView(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event);
};
