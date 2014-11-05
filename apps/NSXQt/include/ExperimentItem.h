#ifndef EXPERIMENTITEM_H
#define EXPERIMENTITEM_H

#include <QStandardItem>

#include <string>

class ExperimentItem : public QStandardItem
{
public:
    explicit ExperimentItem(const std::string& name);

signals:

public:
    void appendRow(QStandardItem *item);

};

#endif // EXPERIMENTITEM_H
