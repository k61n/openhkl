#ifndef INSTRUMENTITEM_H
#define INSTRUMENTITEM_H

#include <QStandardItem>

#include <string>

class InstrumentItem : public QStandardItem
{
public:
    explicit InstrumentItem(const std::string& name);

signals:

public:
    void appendRow(QStandardItem *item);

};

#endif // INSTRUMENTITEM_H
