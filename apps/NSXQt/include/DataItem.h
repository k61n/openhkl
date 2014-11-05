#ifndef DATAITEM_H
#define DATAITEM_H

#include <QStandardItem>

#include <string>

class DataItem : public QStandardItem
{
public:
    explicit DataItem(const std::string& name);

signals:

public slots:

};

#endif // DATAITEM_H
