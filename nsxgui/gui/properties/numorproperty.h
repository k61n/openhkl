
#ifndef NSXGUI_GUI_PROPERTIES_NUMORPROPERTY_H
#define NSXGUI_GUI_PROPERTIES_NUMORPROPERTY_H

#include <QCR/widgets/views.h>
#include <QTableWidget>

class NumorProperty : public QcrWidget {
public:
    NumorProperty();

private:
    void onChanged();
    void onRemake();
    void clear();
    QTableWidget* table;
    QLabel* label;
};

#endif // NSXGUI_GUI_PROPERTIES_NUMORPROPERTY_H
