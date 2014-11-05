#ifndef NUMORITEM_H
#define NUMORITEM_H

#include <QStandardItem>

#include <string>

class NumorItem : public QStandardItem
{
public:
    explicit NumorItem(const std::string& name);

signals:

public slots:

};

#endif // NUMORITEM_H
