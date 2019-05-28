
#ifndef NUMORPROPERTY_H
#define NUMORPROPERTY_H

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

#endif //NUMORPROPERTY_H
